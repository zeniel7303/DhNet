---
description: DhNet_Protocol에 새 패킷을 추가합니다 (PacketEnum + PacketList + 핸들러 스텁)
argument-hint: 추가할 패킷 이름 (예: "RoomKick", "LobbyChat")
---

새 패킷 `$ARGUMENTS`를 추가합니다.

## 절차

### 1. 패킷 방향 결정
`$ARGUMENTS`를 기반으로 방향을 판단합니다.
- 클라이언트 → 서버: `C_$ARGUMENTS`
- 서버 → 클라이언트: `S_$ARGUMENTS`
- 양방향이 필요하면 둘 다 생성합니다.

### 2. PacketEnum.h 수정
`DhNet_Server/DhNet_Protocol/PacketEnum.h`를 읽고 기존 열거형 값 패턴을 확인한 뒤, 적절한 위치에 새 값을 추가합니다.

### 3. PacketList.h 수정
`DhNet_Server/DhNet_Protocol/PacketList.h`를 읽고 기존 패킷 구조체 패턴을 확인한 뒤, `#pragma pack(push, 1)` / `#pragma pack(pop)` 범위 안에 새 구조체를 추가합니다.

### 4. 핸들러 스텁 생성
- 패킷 성격에 맞는 Controller 파일(LobbyController, RoomController, LoginController 등)을 읽고 기존 핸들러 패턴을 확인합니다.
- 해당 Controller `.h`에 함수 선언, `.cpp`에 빈 구현을 추가합니다.
- GameSession 디스패치 등록이 필요하면 위치를 안내합니다.

## 완료 후
수정된 파일 목록과 다음 구현 단계를 요약합니다.
