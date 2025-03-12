#pragma once
#include "IocpCore.h"
#include "NetAddress.h"

class AcceptEvent;

/*----------------------------
		   Listener
------------------------------*/

class Listener : public IocpObject
{
protected:
	SOCKET m_socket = INVALID_SOCKET;
	vector<AcceptEvent*> m_acceptEvents;

public:
	Listener() = default;
	~Listener();

public:
	bool StartAccept(NetAddress _netAddress);
	void CloseSocket();

public:
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* _iocpEvent, __int32 _numOfBytes = 0) override;

private:
	void RegisterAccept(AcceptEvent* _acceptEvent);
	void ProcessAccept(AcceptEvent* _acceptEvent);
};