#pragma once

enum PacketEnum : unsigned __int16
{
	Test = 0,

	Req_Login = 1,
	Res_Login = 2,

	Req_RoomEnter = 3,
	Noti_RoomEnter = 4,

	Req_RoomChat = 5,
	Noti_RoomChat = 6,

	Req_RoomExit = 7,
	Noti_RoomExit = 8,
};