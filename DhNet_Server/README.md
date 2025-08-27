# DhNet Server / Web / IPC 프로젝트 안내서

중요: 레포지토리를 클론한 직후, 가장 먼저 setup.bat을 실행해야합니다. vcpkg가 준비되지 않으면 gRPC와 protobuf 의존성 때문에 C++ 빌드 되지 않습니다.

## 개요
이 저장소는 C++ 네트워킹 서버와 .NET 8 구성요소로 이루어진 솔루션입니다.

구성 요소:
- C++: ServerCore, DhNet_Server, DhNet_Protocol
- .NET 8: DhNet_Ipc (라이브러리/도구), DhNet_Web (ASP.NET Core)

솔루션 파일: DhNet_Server.sln

## 폴더 구조 (요약)
- ServerCore: 소켓, 세션, 버퍼 등 핵심 서버 인프라 (C++)
- DhNet_Server: 서버 애플리케이션 (C++)
- DhNet_Protocol: 프로토콜 (패킷 정보) (C++)
- DhNet_Ipc: .NET 8 IPC/유틸 도구 (C#)
- DhNet_Web: .NET 8 Web/API/관리 페이지(swagger) (C#)

빌드 산출물 : Binary\Debug\DhNet_Server.exe

## 빠른 시작

1) 작업 환경
- Windows 10 이상, x64
- Visual Studio 2022, Rider 2025.2
- .NET SDK 8.0
- Git

2) 처음 한 번만: setup.bat 실행 (반드시 먼저)
- 레포지토리 루트에서 setup.bat을 더블클릭하거나 콘솔에서 실행합니다.
- 이 스크립트는 다음을 수행하도록 설계되어 있습니다:
  - vcpkg 설치 또는 위치 확인
  - bootstrap-vcpkg.bat 실행
  - vcpkg install grpc:x64-windows 실행 (grpc, protobuf 설치)

3) 솔루션 열기 및 빌드
- DhNet_Server.sln을 Visual Studio 2022 또는 JetBrains Rider로 엽니다.
- 구성(Configuration): Debug x64 (기본)
- DhNet_Ipc에 generated 폴더 존재 확인. 없다면 DhNet_Ipc의 tools의 generate_protos.ps1 실행 (dhnet.grpc.pb, dhnet.pb 2개가 .cc, .h 생성되어있는지 확인)
- DhNet_Server 프로젝트 빌드 시 순서 안내:
  1. DhUtil (c++)
  2. ServerCore (C++)
  3. DhNet_Server (C++)
  
4) 실행
- IPC/도구: DhNet_Ipc는 필요 시 별도로 실행합니다. (generate_protos.ps1 실행)
- C++ 서버: DhNet_Server를 시작하여 서버 프로세스를 구동합니다.
- 웹: DhNet_Web을 실행하여 관리/테스트용 웹 인터페이스를 구동합니다.