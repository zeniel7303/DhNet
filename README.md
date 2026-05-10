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
| DB | MySQL C API |
| 암호화 | OpenSSL (AES-128-GCM, ECDH) |
| Admin IPC | gRPC / Protocol Buffers |
| 패키지 관리 | vcpkg |
| 빌드 | Visual Studio 2026 / MSBuild |

---

## 아키텍처

```
┌─────────────────────────────────────────────────────┐
│                    Client                           │
└────────────────────┬────────────────────────────────┘
                     │ TCP (Custom Binary Protocol)
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
│              Admin Layer (DhNet_Ipc)                │
│  gRPC AdminService (HealthCheck / ListRooms / ...)  │
└─────────────────────────────────────────────────────┘
```

### 동시성 모델

- **JobQueue 직렬화** — `Lobby`, `Room`은 `JobQueue`를 상속하며 내부 상태를 단일 워커 스레드에서 순차 처리합니다. 외부에서는 반드시 `DoAsync()`로 호출합니다.
- **RW SpinLock** — `PlayerSystem`, `LobbySystem` 등 여러 스레드가 공유하는 컨테이너는 `USE_LOCK` / `READ_LOCK` / `WRITE_LOCK` 매크로로 보호합니다.
- **Lock-Free CAS** — 로비/룸 슬롯 예약은 `atomic<int32>`와 `compare_exchange_weak`로 TOCTOU 없이 처리합니다.

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
├── external/vcpkg           # 패키지 매니저 (서브모듈)
├── vcpkg.json               # 의존성 버전 고정
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

## 플레이어 생명주기

```
접속
 └→ OnConnected → GameSessionSystem 등록
     └→ [로그인] → PlayerSystem 등록 → AssignLobby
         └→ Lobby::Enter (DoAsync)
             ├→ NotiLobbyPlayerEnter (기존 멤버에게)
             └→ ResLobbyEnter (본인에게, 전체 플레이어 목록 포함)

[룸 입장]
 └→ LeaveLobby → NotiLobbyPlayerExit 브로드캐스트
     └→ Room::Enter (DoAsync) → NotiRoomEnter 브로드캐스트

[룸 퇴장]
 └→ Room::Leave → NotiRoomExit 브로드캐스트
     └→ AssignLobby → Lobby::Enter (DoAsync)

접속 해제
 └→ LeaveLobby → LeaveRoom → PlayerSystem::Remove
     └→ GameSessionSystem::Remove
```

---

## 빌드

### 요구 사항
- Visual Studio 2026 (v145 toolset)
- vcpkg (서브모듈로 포함)

### 의존성 설치

```powershell
# vcpkg 부트스트랩 (최초 1회)
cd external\vcpkg
.\bootstrap-vcpkg.bat -disableMetrics

# 패키지 설치
.\vcpkg.exe install grpc:x64-windows openssl:x64-windows
```

### 빌드 명령

```powershell
& "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" `
  "DhNet_Server\DhNet_Server.sln" `
  /p:Configuration=Debug /p:Platform=x64 /p:PlatformToolset=v145
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

## 개발 로드맵

| Phase | 기능 | 상태 |
|-------|------|------|
| Phase 1 | 로비/룸 시스템, 동적 클러스터링, 채팅 | ✅ 완료 |
| Phase 2 | MySQL C API ConnectionPool, PBKDF2-SHA256 인증 | ✅ 완료 |
| Phase 3 | OpenSSL AES-128-GCM 패킷 암호화 (ECDH 키 교환) | 예정 |
| Phase 4 | cpp-httplib REST API (관리 엔드포인트) | 예정 |
| Phase 5 | 부하 테스트 인프라 (시나리오 기반 스트레스 테스트) | 예정 |
