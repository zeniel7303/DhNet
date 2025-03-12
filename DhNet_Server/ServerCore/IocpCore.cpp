#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

IocpCore::IocpCore()
{
	m_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(m_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(m_iocpHandle);
}

bool IocpCore::Register(IocpObject* _iocpObject)
{
	return ::CreateIoCompletionPort(_iocpObject->GetHandle(), m_iocpHandle, reinterpret_cast<ULONG_PTR>(_iocpObject), 0);
}

bool IocpCore::Dispatch(unsigned __int32 _timeoutMs)
{
	DWORD numOfBytes = 0;
	IocpObject* iocpObject = nullptr;
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(m_iocpHandle, OUT & numOfBytes,
		OUT reinterpret_cast<PULONG_PTR>(&iocpObject),
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), _timeoutMs))
	{
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		__int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}
