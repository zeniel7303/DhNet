#pragma once
#include "../ServerCore/Session.h"

class GameSession : public Session
{
private:


public:
	~GameSession()
	{
		cout << "~GameSession" << endl;
	}

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual int32 OnRecv(BYTE* _buffer, int32 _len) override;
	virtual void OnSend(int32 _len) override;
};