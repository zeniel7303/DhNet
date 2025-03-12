#pragma once

/*--------------
   IocpObject
---------------*/

// Listener과 Session의 부모 추상 클래스
// IOCP에서의 Session 역할 담당

class IocpObject
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* _iocpEvent, __int32 _numOfBytes = 0) abstract;
};

/*--------------
	IocpCore
---------------*/

class IocpCore
{
private:
	HANDLE		m_iocpHandle;

public:
	IocpCore();
	~IocpCore();

	HANDLE		GetHandle() { return m_iocpHandle; }

	// IOCP에 관찰 대상으로 등록
	bool		Register(IocpObject* _iocpObject);
	// WorkerThread들이 IOCP에 일거리를 찾는 함수
	bool		Dispatch(unsigned __int32 _timeoutMs = INFINITE);
};