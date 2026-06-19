#pragma once

struct ScenarioConfig
{
    int lobbyChatCount = 2;
    int lobbyChatIntervalMs = 1000;
    int roomChatCount = 5;
    int roomChatIntervalMs = 1000;
    int roomActionTimeoutMs = 5000; // Req_RoomEnter/Req_RoomExit 응답(Noti/Res_LobbyEnter)이 안 올 때 재시도 간격
    int maxRoomActionRetries = 3;   // 재시도 한도 — 서버가 Req_RoomEnter를 멱등하게 처리하지 않으므로
                                     // 무한 재시도 시 중복 Room::Enter race를 유발할 수 있음. 한도 초과 시 Stuck 처리.

    // RTT 측정용 핑(Test 패킷, PacketEnum::Test). 로비/룸 채팅은 서버가 응답을 안 보내고
    // 룸 입장/퇴장은 사이클당 2번뿐이라 샘플이 희소함 — 게임 시나리오와 독립적으로
    // 별도 핑을 주기적으로 쏴서 RTT 표본을 충분히 확보한다. Test 패킷은 상태를 바꾸지 않는
    // 순수 echo라 무한 재시도해도 서버 측 race 위험이 없음(Req_RoomEnter와 다른 점).
    int pingIntervalMs = 200;
    int pingTimeoutMs = 3000;
};

extern ScenarioConfig g_scenarioConfig;
