#include "stdafx.h"
#include "GameSession.h"
#include "GameSessionManager.h"

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}

int32 GameSession::OnRecv(BYTE* _buffer, int32 _len)
{
	cout << "OnRecv Len " << _len << endl;
	Send(_buffer, _len);
	return _len;
}

void GameSession::OnSend(int32 _len)
{
	cout << "OnSend Len " << _len << endl;
}