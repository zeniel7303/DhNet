#pragma once

enum PacketEnum : unsigned __int16
{
	Test = 0,

	Req_Login = 1,
	Res_Login = 2,

	Req_RoomEnter = 3,
	Res_RoomEnter = 4,
};