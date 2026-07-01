# DhNet GameServer - Claude Code 가이드

## 프로젝트 개요

C++ IOCP 게임 서버 + C# ASP.NET Core 관리 REST API로 구성된 멀티플레이어 게임 서버.  
C++ `AdminGrpcServer`와 C# `DhNet_Web` 사이를 gRPC로 연결하는 구조.

**C++ 기술 스택**: C++17, IOCP(ServerCore), 커스텀 패킷 프로토콜(헤더 기반), gRPC C++(Admin), MySQL, OpenSSL(AES-GCM/ECDH)  
**C# 기술 스택**: .NET 8, ASP.NET Core(DhNet_Web), Grpc.Net.Client(DhNet_Ipc)  
**빌드**: MSBuild (C++/C# 통합 — `DhNet_Server.sln`)

## 프로젝트 구조

```
DhNet/
├── DhNet_Server/
│   ├── DhNet_Server.sln              # C++/C# 통합 솔루션
│   ├── DhNet_Server/                 # C++ 게임 서버 코어
│   │   ├── GameSession, GameSessionSystem
│   │   ├── Lobby, LobbySystem, LobbyController
│   │   ├── Room, RoomSystem, RoomController
│   │   ├── Player, PlayerSystem
│   │   ├── AdminGrpcServer, AdminController  # gRPC 관리 서버 (C++ 내장)
│   │   ├── LoginController, AccountRepository
│   │   ├── DbConnection, DbConnectionPool, DbSystem
│   │   ├── CryptoUtil, AesGcm, EcdhKeyExchange
│   │   └── ServerGlobal, ServerSetting, UniqueIdGenerationSystem
│   ├── ServerCore/                   # C++ IOCP 네트워킹 레이어
│   ├── DhNet_Protocol/               # C++ 커스텀 패킷 프로토콜 헤더
│   │   └── PacketList.h, PacketEnum.h, ErrorCode.h
│   ├── DhNet_Ipc/                    # C# .NET 8 — dhnet.proto + gRPC 클라이언트 코드
│   └── DhNet_Web/                    # C# .NET 8 ASP.NET Core REST API
│       ├── Controllers/              # Players, Lobbies, Rooms, Health
│       └── Services/GrpcAdminClient  # DhNet_Ipc를 통해 C++ gRPC 서버에 연결
├── DhNet_Client/
│   ├── DhNet_Client.sln
│   ├── DhNet_Client/
│   └── DhNet_StressTest/
├── DhUtil/                           # C++ 유틸리티 라이브러리
│   └── Lock(RW SpinLock), ThreadManager, TLS, JobQueue, ObjectPool, DeadLockProfiler
├── Binary/Debug/                     # 빌드 출력 + 런타임 DLL
├── docker/
└── Tools/
```

## .claude/ 인프라 시스템

**항상 이 시스템을 활용하여 작업한다.**

### 에이전트
- **code-architecture-reviewer** - 코드 리뷰 요청 시 사용
- **refactor-planner** - 리팩토링 계획 수립 시 사용
- **documentation-architect** - 문서 생성 시 사용
- **plan-reviewer** - 구현 계획 검토 시 사용

### 커스텀 명령
- `/dev-docs [작업명]` - 새 작업 시작 시 `dev/active/[작업명]/` 구조 생성
- `/dev-docs-update [작업명]` - 현재 작업 상태 저장
- `/new-packet [패킷명]` - PacketEnum.h + PacketList.h + 핸들러 스텁 한번에 생성
- `/new-grpc [메서드명]` - dhnet.proto + C++ AdminGrpcServer + C# GrpcAdminClient 스텁 생성
- `/build` - DhNet_Server.sln 즉시 빌드

## 빌드 시스템

### MSBuild — C++/C# 통합 (git-bash에서)
```bash
MSBUILD="/c/Program Files/Microsoft Visual Studio/18/Professional/MSBuild/Current/Bin/MSBuild.exe"
"$MSBUILD" DhNet_Server/DhNet_Server.sln -p:Configuration=Debug -p:Platform=x64 -m -nologo -v:minimal
```
**주의**: `/p:` 형식은 MSYS 경로 변환으로 오작동. 반드시 `-p:` 형식 사용.  
`DhNet_Server.sln`이 C++ (DhNet_Server, ServerCore, DhUtil, DhNet_Protocol)과 C# (DhNet_Ipc, DhNet_Web)을 모두 포함하므로 한 번에 빌드됨.

### dotnet — C# 단독 빌드
```bash
dotnet build DhNet_Server/DhNet_Web/DhNet_Web.csproj
```

### vcpkg
- manifest-mode: `vcpkg install --triplet x64-windows` (repo root에서)
- 설치 경로: `vcpkg_installed/x64-windows/` (junction → `external/vcpkg/installed/`)
- protoc: `external/vcpkg/installed/x64-windows/tools/protobuf/protoc.exe`

**새 PC 셋업 시 junction 재생성 필요** (junction은 로컬 전용, git으로 안 따라옴):
```powershell
# PowerShell에서 실행 (repo root에서)
New-Item -ItemType Junction -Path "external/vcpkg/installed" -Target "$PWD/vcpkg_installed"
```
`external/vcpkg/installed`가 이미 존재하면 먼저 삭제 후 재생성. git-bash의 `mklink /J`는 오작동하므로 반드시 PowerShell 사용.

### dhnet.proto 재생성
`DhNet_Server/DhNet_Ipc/tools/generate_protos.ps1` 실행. `.proto` 수정 시 C++ generated 파일(`dhnet.pb.cc/h`, `dhnet.grpc.pb.cc/h`)도 함께 재생성 필요.

### 런타임 DLL
`Binary/Debug/`에 필요: abseil_dll, cares, libcrypto-3-x64, libprotobufd, libssl-3-x64, re2, zlibd1, libmysql, zd, zstd, spdlogd, fmtd  
vcpkg 업데이트 후 DLL 교체 필요.

## 코딩 컨벤션

### 네이밍
- 클래스/함수: PascalCase
- 멤버 변수: `m_camelCase`
- 로컬 변수: camelCase

### C++ 필수 패턴

```cpp
// 1. 동시성: DhUtil의 Lock(RW SpinLock) + USE_LOCK 매크로
class LobbySystem {
    USE_LOCK;
    unordered_map<uint32, shared_ptr<Lobby>> m_lobbies;

    void AddLobby(shared_ptr<Lobby> lobby) {
        WRITE_LOCK;
        m_lobbies[lobby->GetId()] = lobby;
    }
};

// 2. 교차 참조: weak_ptr (순환 참조 방지)
class Player {
    weak_ptr<GameSession> m_session;   // GameSession → Player → GameSession 순환 방지
    weak_ptr<Room> m_currentRoom;
};

// 3. RAII + enable_shared_from_this
class GameSession : public PacketSession, public enable_shared_from_this<GameSession> {
public:
    ~GameSession();
};
```

### C++ 금지 사항
- `shared_ptr` 순환 참조 (GameSession ↔ Player, Lobby ↔ Room 주의)
- `concurrent_unordered_map` (성능 저하 이슈 — `unordered_map + Lock` 사용)
- raw new/delete

### C# (DhNet_Web) 필수 패턴

```csharp
// 1. 컨트롤러: async/await + nullable 활성화
public async Task<IActionResult> GetPlayers() {
    var result = await _adminClient.GetPlayersAsync();
    return Ok(result);
}

// 2. gRPC 클라이언트: DhNet_Ipc 통해 C++ 서버 호출
public class GrpcAdminClient : IAdminClient {
    private readonly AdminService.AdminServiceClient _client;
}
```

## 작업 워크플로우

작업 워크플로우 규칙은 `dev/CLAUDE.md` 참고.

## Claude Workspace (다중 PC 동기화)

`dev/`와 Claude 메모리는 private `claude-workspace` 레포를 통해 기기 간 동기화된다.  
새 PC 셋업 시 `README.md` **"Claude Workspace 초기 설정"** 섹션 참고.
