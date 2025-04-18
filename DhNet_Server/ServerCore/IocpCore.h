#pragma once

// Listener과 Session의 부모 추상 클래스
// IOCP에서의 Session 역할 담당

// https://stackoverflow.com/questions/11711034/stdshared-ptr-of-this
class IocpObject : public std::enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* _iocpEvent, int32 _numOfBytes = 0) abstract;
};

class IocpCore
{
private:
	HANDLE		m_iocpHandle;

public:
	IocpCore();
	~IocpCore();

	HANDLE		GetHandle() { return m_iocpHandle; }

	// IOCP에 관찰 대상으로 등록
	bool		Register(IocpObjectRef _iocpObject);
	// WorkerThread들이 IOCP에 일거리를 찾는 함수
	bool		Dispatch(uint32 _timeoutMs = INFINITE);
};