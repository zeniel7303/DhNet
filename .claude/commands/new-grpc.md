---
description: dhnet.proto에 새 gRPC 메서드를 추가하고 C++/C# 양쪽 스텁을 생성합니다
argument-hint: 추가할 RPC 메서드 이름 (예: "KickPlayer", "GetRoomList")
---

새 gRPC 메서드 `$ARGUMENTS`를 추가합니다.

## 절차

### 1. proto 파일 읽기
`DhNet_Server/DhNet_Ipc/dhnet.proto`를 읽고 기존 message와 service 정의 패턴을 확인합니다.

### 2. dhnet.proto 수정
- `$ARGUMENTS`Request / `$ARGUMENTS`Response message 타입 추가
- `AdminService` service에 새 rpc 메서드 추가

### 3. proto 재생성 안내
proto 수정 후 반드시 아래를 실행해야 합니다:
```
DhNet_Server/DhNet_Ipc/tools/generate_protos.ps1
```
C++ generated 파일(`dhnet.pb.cc/h`, `dhnet.grpc.pb.cc/h`)도 함께 재생성됩니다.

### 4. C++ AdminGrpcServer 스텁
`DhNet_Server/DhNet_Server/AdminGrpcServer.cpp`와 `.h`를 읽고 기존 메서드 패턴을 확인한 뒤, 아래 형태의 스텁을 추가합니다:
```cpp
grpc::Status $ARGUMENTS(grpc::ServerContext* context,
                         const dhnet::$ARGUMENTS Request* request,
                         dhnet::$ARGUMENTS Response* response) override;
```

### 5. C# GrpcAdminClient 스텁
`DhNet_Server/DhNet_Web/Services/GrpcAdminClient.cs`를 읽고 기존 메서드 패턴을 확인한 뒤, 아래 형태의 스텁을 추가합니다:
- `async Task<...> $ARGUMENTS Async(...)` 구현
- `RpcException` 처리 포함
- `IAdminClient` 인터페이스가 존재하면 인터페이스에도 메서드 선언 추가

## 완료 후
수정된 파일 목록과 proto 재생성 후 필요한 후속 작업(빌드 등)을 안내합니다.
