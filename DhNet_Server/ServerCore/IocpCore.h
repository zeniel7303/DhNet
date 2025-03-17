#pragma once

/*--------------
   IocpObject
---------------*/

// Listener�� Session�� �θ� �߻� Ŭ����
// IOCP������ Session ���� ���

// https://stackoverflow.com/questions/11711034/stdshared-ptr-of-this
class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* _iocpEvent, int32 _numOfBytes = 0) abstract;
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
	bool		Register(shared_ptr<IocpObject> _iocpObject);
	// WorkerThread���� IOCP�� �ϰŸ��� ã�� �Լ�
	bool		Dispatch(uint32 _timeoutMs = INFINITE);
};