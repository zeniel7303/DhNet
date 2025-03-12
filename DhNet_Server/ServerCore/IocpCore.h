#pragma once

/*--------------
   IocpObject
---------------*/

// Listener�� Session�� �θ� �߻� Ŭ����
// IOCP������ Session ���� ���

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

	// IOCP�� ���� ������� ���
	bool		Register(IocpObject* _iocpObject);
	// WorkerThread���� IOCP�� �ϰŸ��� ã�� �Լ�
	bool		Dispatch(unsigned __int32 _timeoutMs = INFINITE);
};