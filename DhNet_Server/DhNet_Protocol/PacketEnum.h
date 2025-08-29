#pragma once

enum PacketEnum : unsigned __int16
{
	Test = 0,

	Req_Login = 1,
	Res_Login = 2,

	Req_RoomEnter = 3,
	Res_RoomEnter = 4,
	Noti_RoomEnter = 5,

	Req_RoomChat = 6,
	Noti_RoomChat = 7,

	Req_RoomExit = 8,
	Res_RoomExit = 9,
	Noti_RoomExit = 10,

	Req_LobbyChat = 11,
	Noti_LobbyChat = 12,
};