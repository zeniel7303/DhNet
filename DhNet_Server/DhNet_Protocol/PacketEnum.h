#pragma once

enum PacketEnum : unsigned __int16
{
	// ── 시스템 (0~9) ─────────────────────────────────
	Test = 0,

	// ── 인증 (10~29) ─────────────────────────────────
	Req_Login      = 10,
	Res_Login      = 11,
	Res_LoginFailed = 12,

	// ── 로비 (30~59) ─────────────────────────────────
	Res_LobbyEnter        = 30,
	Noti_LobbyPlayerEnter = 31,
	Noti_LobbyPlayerExit  = 32,
	Req_LobbyChat         = 33,
	Noti_LobbyChat        = 34,
	Req_RoomList          = 35,
	Res_RoomList          = 36,

	// ── 룸 (60~99) ────────────────────────────────────
	Req_RoomEnter  = 60,
	Res_RoomEnter  = 61,
	Noti_RoomEnter = 62,
	Req_RoomChat   = 63,
	Noti_RoomChat  = 64,
	Req_RoomExit   = 65,
	Res_RoomExit   = 66,
	Noti_RoomExit  = 67,
};