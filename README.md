# DhNet Game Server

Windows IOCP 기반 C++ 멀티플레이어 게임 서버 프레임워크.

고성능 비동기 네트워킹, 로비/룸 시스템, MySQL 인증, AES 암호화, gRPC 관리 인터페이스를 목표로 개발 중입니다.

---

## 기술 스택

| 분류 | 기술 |
|------|------|
| 언어 | C++17 |
| 네트워크 | Windows IOCP |
| 프로토콜 | Custom Binary (Little-Endian, 4-byte header) |
| DB | MySQL 8.0 (C API) |
| 암호화 | OpenSSL (PBKDF2-SHA256 인증, AES-128-GCM + ECDH P-256 세션 암호화) |
| Admin IPC | gRPC / Protocol Buffers |
| 패키지 관리 | vcpkg |
| 빌드 | Visual Studio 2026 / MSBuild |
| 컨테이너 | Docker (MySQL) |

---

## 아키텍처

```
┌─────────────────────────────────────────────────────┐
│                    Client                           │
└────────────────────┬────────────────────────────────┘
                     │ TCP (Custom Binary Protocol, port 7900)
┌────────────────────▼────────────────────────────────┐
│              Network Layer (ServerCore)             │
│  IocpCore → Listener → Session → RecvBuffer         │
│                       ↓ PacketHandler               │
├─────────────────────────────────────────────────────┤
│               Logic Layer (DhNet_Server)            │
│                                                     │
│  ┌──────────┐  ┌──────────┐  ┌──────────────────┐  │
│  │  Lobby   │  │   Room   │  │  PlayerSystem    │  │
│  │ JobQueue │  │ JobQueue │  │  (RW SpinLock)   │  │
│  └──────────┘  └──────────┘  └──────────────────┘  │
│                                                     │
│  LobbySystem / RoomSystem / GameSessionSystem       │
├─────────────────────────────────────────────────────┤
│               DB Layer (DbSystem)                   │
│  DbConnectionPool (MySQL C API) + Worker Thread     │
├─────────────────────────────────────────────────────┤
│              Admin Layer (DhNet_Ipc)                │
│  gRPC AdminService (port 7820)                      │
└─────────────────────────────────────────────────────┘
```

### 동시성 모델

- **JobQueue 직렬화** — `Lobby`, `Room`은 `JobQueue`를 상속하며 내부 상태를 단일 워커 스레드에서 순차 처리합니다. 외부에서는 반드시 `DoAsync()`로 호출합니다.
- **RW SpinLock** — `PlayerSystem`, `LobbySystem` 등 여러 스레드가 공유하는 컨테이너는 `USE_LOCK` / `READ_LOCK` / `WRITE_LOCK` 매크로로 보호합니다.
- **DB 비동기 처리** — 네트워크 스레드에서 DB를 직접 호출하지 않습니다. `DbSystem`의 전용 워커 스레드 풀이 쿼리를 실행하고, 결과는 `PushGlobalQueue`로 IOCP 스레드에 복귀합니다.

---

## 프로젝트 구조

```
DhNet/
├── DhNet_Server/
│   ├── ServerCore/          # IOCP 네트워크 레이어
│   │   ├── IocpCore         # IOCP 이벤트 루프
│   │   ├── Listener         # Accept 처리
│   │   ├── Session          # 연결 세션 (RecvBuffer / SendBuffer)
│   │   ├── PacketHandler    # 패킷 ID → 핸들러 디스패치
│   │   └── SendPool         # 멀티청크 송신 버퍼 풀
│   ├── DhNet_Protocol/      # 패킷 정의
│   │   ├── PacketHeader.h   # uint16 size + uint16 id (4 bytes)
│   │   ├── PacketEnum.h     # 패킷 ID 열거
│   │   └── PacketList.h     # Req / Res / Noti 구조체
│   ├── DhNet_Ipc/           # gRPC Admin 인터페이스
│   │   ├── dhnet.proto
│   │   └── generated/       # protoc 생성 파일
│   └── DhNet_Server/        # 게임 서버 로직
│       ├── GameServer        # 싱글톤, 시스템 컨테이너
│       ├── ServerSetting     # 환경변수 기반 서버 설정
│       ├── DbSystem          # DB 워커 스레드 풀
│       ├── DbConnectionPool  # MySQL C API 커넥션 풀
│       ├── AccountRepository # 인증 쿼리 (Prepared Statement)
│       ├── CryptoUtil        # PBKDF2-SHA256 해시/검증
│       ├── AesGcm            # AES-128-GCM 암복호화 (헤더 전용)
│       ├── EcdhKeyExchange   # ECDH P-256 키 교환 + SHA-256 세션 키 유도
│       ├── Lobby / LobbySystem / LobbyController
│       ├── Room  / RoomSystem  / RoomController
│       ├── Player / PlayerSystem
│       ├── LoginController
│       └── AdminGrpcServer
├── DhUtil/                  # 공통 유틸리티
│   ├── JobQueue             # 비동기 작업 직렬화
│   ├── Lock                 # RW SpinLock
│   ├── ObjectPool           # 메모리 재사용
│   ├── ThreadManager        # 워커 스레드 풀
│   └── GlobalQueue          # 크로스-스레드 작업 큐
├── DhNet_Client/            # 테스트 클라이언트
├── docker/                  # MySQL Docker 설정
│   ├── docker-compose.yml
│   └── mysql/init/01_schema.sql
├── external/vcpkg           # 패키지 매니저 (서브모듈)
├── vcpkg.json               # 의존성 정의
└── Binary/                  # 빌드 결과물
```

---

## 패킷 프로토콜

모든 패킷은 `uint16 size + uint16 id` 4바이트 헤더로 시작합니다.

```
┌────────────┬────────────┬─────────────────┐
│  size (2B) │   id (2B)  │   payload ...   │
└────────────┴────────────┴─────────────────┘
```

| 접두사 | 방향 | 예시 |
|--------|------|------|
| `Req`  | Client → Server | `ReqLogin`, `ReqRoomEnter` |
| `Res`  | Server → Client | `ResLogin`, `ResLobbyEnter` |
| `Noti` | Server → Client (단방향) | `NotiLobbyChat`, `NotiRoomExit` |

<details>
<summary>구현된 패킷 목록</summary>

| ID | 이름 | 설명 |
|----|------|------|
| 1  | `Req_KeyExchange` | ECDH 공개키 전송 (핸드셰이크 시작) |
| 2  | `Res_KeyExchange` | 서버 공개키 응답 (핸드셰이크 완료) |
| 3  | `Encrypted` | 암호화 래퍼 (핸드셰이크 후 모든 패킷) |
| 10 | `Req_Login` | 로그인 요청 |
| 11 | `Res_Login` | 로그인 응답 |
| 12 | `Res_LoginFailed` | 로그인 실패 응답 |
| 30 | `Res_LobbyEnter` | 로비 입장 응답 |
| 31 | `Noti_LobbyPlayerEnter` | 로비 입장 알림 |
| 32 | `Noti_LobbyPlayerExit` | 로비 퇴장 알림 |
| 33 | `Req_LobbyChat` | 로비 채팅 요청 |
| 34 | `Noti_LobbyChat` | 로비 채팅 알림 |
| 35 | `Req_RoomList` | 룸 목록 요청 |
| 36 | `Res_RoomList` | 룸 목록 응답 |
| 60 | `Req_RoomEnter` | 룸 입장 요청 |
| 61 | `Res_RoomEnter` | 룸 입장 응답 |
| 62 | `Noti_RoomEnter` | 룸 입장 알림 |
| 63 | `Req_RoomChat` | 룸 채팅 요청 |
| 64 | `Noti_RoomChat` | 룸 채팅 알림 |
| 65 | `Req_RoomExit` | 룸 퇴장 요청 |
| 66 | `Res_RoomExit` | 룸 퇴장 응답 |
| 67 | `Noti_RoomExit` | 룸 퇴장 알림 |

</details>

---

## 암호화 핸드셰이크 (ECDH + AES-128-GCM)

TCP 연결 직후 키 교환을 완료하며, 이후 모든 패킷은 `EncryptedPacket`으로 래핑됩니다.

```
Client                              Server
  │                                   │
  │  1. GenerateKeyPair (P-256)        │
  │── ReqKeyExchange ─────────────────►│  clientPubKey[65]
  │   (평문 전송)                      │
  │                                   │  2. GenerateKeyPair (P-256)
  │                                   │  3. DeriveSharedSecret (ECDH)
  │                                   │  4. DeriveSessionKey (SHA-256)
  │                                   │     "DhNet-session-key" || secret → AES key 16B
  │◄── ResKeyExchange ────────────────│  serverPubKey[65]
  │   (평문 전송)                      │
  │                                   │
  │  5. DeriveSharedSecret (ECDH)      │
  │  6. DeriveSessionKey (SHA-256)     │
  │     → 동일한 AES-128 세션 키       │
  │                                   │
  │  ✅ 핸드셰이크 완료 — 이후 모든 패킷은 EncryptedPacket으로 래핑
  │                                   │
  │── EncryptedPacket[ReqLogin] ──────►│  nonce(12) | ciphertext | tag(16)
  │◄── EncryptedPacket[ResLogin] ─────│
  │           ...                     │
```

`EncryptedPacket` 구조:
```
┌────────────┬────────────┬──────────────┬──────────────────────┬──────────┐
│  size (2B) │  id=3 (2B) │  nonce (12B) │  ciphertext (plain)  │ tag (16B)│
└────────────┴────────────┴──────────────┴──────────────────────┴──────────┘
```

---

## 로그인 인증 프로세스

클라이언트가 `ReqLogin`을 전송하면 다음 흐름으로 처리됩니다.

```
Client                    ServerCore              LoginController           DbSystem (Worker Thread)
  │                           │                        │                           │
  │── ReqLogin ──────────────►│                        │                           │
  │   username[16]            │── PacketHandler ──────►│                           │
  │   password[64]            │                        │ 1. 길이 검증               │
  │                           │                        │    username: 4~16자        │
  │                           │                        │    password: 8~64자        │
  │                           │                        │ 2. DbSystem::Execute ─────►│
  │                           │                        │                           │ 3. DbConnection 획득
  │                           │                        │                           │    (ConnectionPool)
  │                           │                        │                           │
  │                           │                        │                           │ 4. AccountRepository
  │                           │                        │                           │    SELECT id,
  │                           │                        │                           │      password_hash,
  │                           │                        │                           │      salt
  │                           │                        │                           │    WHERE username=?
  │                           │                        │                           │    (Prepared Statement)
  │                           │                        │                           │
  │                           │                        │                           │ 5. CryptoUtil::VerifyPassword
  │                           │                        │                           │    PBKDF2-SHA256
  │                           │                        │                           │    100,000 iterations
  │                           │                        │                           │    CRYPTO_memcmp (상수 시간)
  │                           │                        │                           │
  │                           │◄── PushGlobalQueue ────│◄──────────────────────────│
  │                           │    (IOCP 스레드 복귀)   │                           │
  │                           │                        │                           │
  │                           │       GameSession::OnLoginResult                   │
  │                           │                        │                           │
  │                           │    ┌─ 인증 실패 ────────┤                           │
  │◄── ResLoginFailed ────────│    │                   │                           │
  │    (연결 종료)             │    └─ 인증 성공 ────────┤                           │
  │                           │                        │ Player 생성               │
  │                           │                        │ PlayerSystem::Add         │
  │◄── ResLogin ──────────────│                        │ Lobby::Enter (DoAsync)    │
  │    playerId, playerName   │                        │                           │
  │◄── ResLobbyEnter ─────────│                        │                           │
  │    lobbyIndex, players[]  │                        │                           │
```

### 보안 설계

| 항목 | 구현 |
|------|------|
| 비밀번호 저장 | PBKDF2-SHA256, 랜덤 salt 16바이트, 100,000 iterations |
| Timing Attack 방어 | 존재하지 않는 사용자명 조회 시에도 dummy hash로 VerifyPassword 실행 (응답 시간 균일화) |
| User Enumeration 방어 | 사용자명 없음 / 비밀번호 불일치 모두 동일한 `ResLoginFailed` 응답 |
| SQL Injection 방어 | Prepared Statement 전용 사용 (`mysql_stmt_*` API) |
| 상수 시간 비교 | `CRYPTO_memcmp`으로 해시 비교 (분기 기반 조기 종료 방지) |
| 패킷 암호화 | ECDH P-256 키 교환 → SHA-256 세션 키 유도 → AES-128-GCM (RAND_bytes nonce, GCM 인증 태그 16B) |

---

## 플레이어 생명주기

```
접속
 └→ OnConnected → GameSessionSystem 등록
     └→ [ReqLogin 수신] → DB 인증 (비동기)
         └→ OnLoginResult(성공) → Player 생성 → PlayerSystem 등록
             └→ Lobby::Enter (DoAsync)
                 ├→ NotiLobbyPlayerEnter (기존 멤버에게)
                 └→ ResLobbyEnter (본인에게, 전체 플레이어 목록 포함)

[ReqRoomEnter 수신]
 └→ LeaveLobby → NotiLobbyPlayerExit 브로드캐스트
     └→ Room::Enter (DoAsync) → NotiRoomEnter 브로드캐스트

[ReqRoomExit 수신]
 └→ Room::Leave → NotiRoomExit 브로드캐스트
     └→ AssignLobby → Lobby::Enter (DoAsync)

접속 해제
 └→ LeaveLobby or LeaveRoom → PlayerSystem::Remove
     └→ GameSessionSystem::Remove
```

---

## 환경변수

서버 설정은 환경변수로 주입합니다. 설정하지 않으면 기본값이 사용됩니다.

| 변수 | 기본값 | 설명 |
|------|--------|------|
| `DhNet_IP` | `127.0.0.1` | 서버 바인드 IP |
| `DhNet_PORT` | `7900` | 게임 서버 포트 |
| `DhNet_MAX_SESSION_COUNT` | `1000` | 최대 동시 세션 수 |
| `DhNet_GRPC_ADDRESS` | `127.0.0.1:7820` | gRPC Admin 서버 주소 |
| `DhNet_DB_HOST` | `127.0.0.1` | MySQL 호스트 |
| `DhNet_DB_PORT` | `3306` | MySQL 포트 |
| `DhNet_DB_USER` | `dhnet` | MySQL 접속 계정 |
| `DhNet_DB_PASSWORD` | `dhnet_pw` | MySQL 비밀번호 |
| `DhNet_DB_NAME` | `dhnet_db` | MySQL 데이터베이스명 |
| `DhNet_DB_POOL_SIZE` | `4` | 커넥션 풀 크기 (= DB 워커 스레드 수) |

---

## 빌드

### 요구 사항

- Visual Studio 2026 (v145 toolset)
- vcpkg (서브모듈로 포함)
- Docker Desktop (MySQL 컨테이너용)

### 의존성 설치

```powershell
# vcpkg 부트스트랩 (최초 1회)
cd external\vcpkg
.\bootstrap-vcpkg.bat -disableMetrics

# 패키지 설치 (grpc, openssl, libmysql)
.\vcpkg.exe install --triplet x64-windows
```

### 빌드 순서

ServerCore → DhNet_Server 순서로 빌드해야 합니다.

```powershell
$msbuild = "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"

# 1. ServerCore 빌드
& $msbuild "DhNet_Server\ServerCore\ServerCore.vcxproj" /p:Configuration=Debug /p:Platform=x64

# 2. ServerCore.lib 복사 (vcxproj OutDir 불일치 워크어라운드)
Copy-Item "DhNet_Server\ServerCore\x64\Debug\ServerCore.lib" "DhNet_Server\x64\Debug\"

# 3. DhNet_Server 빌드
& $msbuild "DhNet_Server\DhNet_Server\DhNet_Server.vcxproj" /p:Configuration=Debug /p:Platform=x64

# 4. 테스트 클라이언트 빌드
& $msbuild "DhNet_Client\DhNet_Client\DhNet_Client.vcxproj" /p:Configuration=Debug /p:Platform=x64
```

### 런타임 DLL 배포

빌드 후 `Binary\Debug\`에 다음 DLL을 복사해야 합니다.

```powershell
$vcpkg_dbg = "external\vcpkg\installed\x64-windows\debug\bin"
$vcpkg_rel = "external\vcpkg\installed\x64-windows\bin"
$dest = "Binary\Debug"

# gRPC / Protobuf / 기타 의존성
Copy-Item "$vcpkg_dbg\abseil_dll.dll"       $dest
Copy-Item "$vcpkg_dbg\cares.dll"            $dest
Copy-Item "$vcpkg_dbg\libcrypto-3-x64.dll" $dest
Copy-Item "$vcpkg_dbg\libssl-3-x64.dll"    $dest
Copy-Item "$vcpkg_dbg\lz4d.dll"            $dest
Copy-Item "$vcpkg_dbg\re2.dll"             $dest
Copy-Item "$vcpkg_dbg\zd.dll"              $dest
Copy-Item "$vcpkg_dbg\zstd.dll"            $dest

# libmysql (release bin에서 복사)
Copy-Item "$vcpkg_rel\libmysql.dll"         $dest
```

> **참고**: boost DLL (`boost_atomic-*.dll` 등)은 libmysql의 트랜지티브 의존성입니다.
> 로컬 개발 환경에서는 vcpkg 설치 경로에서 자동 로드되지만, 다른 PC에 배포할 때는 `external\vcpkg\installed\x64-windows\debug\bin\boost_*.dll`도 함께 복사해야 합니다.

### MySQL Docker 시작

```powershell
cd docker
docker compose up -d

# 접속 확인
docker exec -it dhnet_mysql mysql -u dhnet -pdhnet_pw dhnet_db
```

### protobuf 재생성 (dhnet.proto 수정 시)

```powershell
$protoc  = "external\vcpkg\installed\x64-windows\tools\protobuf\protoc.exe"
$plugin  = "external\vcpkg\installed\x64-windows\tools\grpc\grpc_cpp_plugin.exe"
$out     = "DhNet_Server\DhNet_Ipc\generated"
& $protoc --proto_path=DhNet_Server\DhNet_Ipc --cpp_out=$out --grpc_out=$out `
  "--plugin=protoc-gen-grpc=$plugin" DhNet_Server\DhNet_Ipc\dhnet.proto
```

---

## 테스트 계정 설정

Docker MySQL 시작 후 테스트 계정 비밀번호 해시를 생성해 INSERT해야 합니다.

```python
import hashlib, os

password = "testpass"
salt = os.urandom(16).hex()
hash_val = hashlib.pbkdf2_hmac(
    "sha256", password.encode(), bytes.fromhex(salt), 100000
).hex()
print(f"salt: {salt}")
print(f"hash: {hash_val}")
```

```sql
UPDATE accounts
SET password_hash = '<hash>', salt = '<salt>'
WHERE username = 'testuser';
```

---

## 개발 로드맵

| Phase | 기능 | 상태 |
|-------|------|------|
| Phase 1 | 로비/룸 시스템, 동적 클러스터링, 채팅 | ✅ 완료 |
| Phase 2 | MySQL C API ConnectionPool, PBKDF2-SHA256 인증 | ✅ 완료 |
| Phase 3 | OpenSSL AES-128-GCM 패킷 암호화 (ECDH P-256 키 교환) | ✅ 완료 |
| Phase 4 | cpp-httplib REST API (관리 엔드포인트) | 예정 |
| Phase 5 | 부하 테스트 인프라 (시나리오 기반 스트레스 테스트) | 예정 |
