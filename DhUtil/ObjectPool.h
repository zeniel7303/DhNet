#pragma once
#include "pch.h"
#include <iostream>

template <typename Type>
class ObjectPool
{
private:
	static Lock m_lock;

	static std::queue<Type*>	m_queue;

	static int					m_usedSize;
	static int					m_poolSize;

public:
	template<typename... Args>
	static std::shared_ptr<Type> MakeShared(Args&&... _args)
	{
		/*std::shared_ptr<Type> sptr = { Pop(std::forward<Args>(_args)...), Push };
		return sptr;*/

		Type* memory = Pop(std::forward<Args>(_args)...);
		return std::shared_ptr<Type>(memory, [](Type* obj) { Push(obj); });
	}

private:
	static Type* GetMemory()
	{
		WriteLockGuard writeLockGuard(m_lock, typeid(ObjectPool<Type>).name());

		Type* memory = nullptr;

		if (m_queue.empty() == false)
		{
			//있으면 하나 꺼내온다.
			memory = m_queue.front();
			m_queue.pop();
		}
		else
		{
			//없으면 새로 만든다.
			memory = new Type;
			m_poolSize++;
		}

		m_usedSize++;
		return memory;
	}

	template<typename... Args>
	static Type* Pop(Args&&... args)
	{
		Type* memory = GetMemory();
		ASSERT_CRASH(memory != nullptr);

		try
		{
			new(memory)Type(std::forward<Args>(args)...);
		}
		catch (...)
		{
			// 초기화 실패 시 메모리 반환
			Push(memory);
			throw;
		}

#if _DEBUG
		// std::cout << "ObjectPool::GetMemory() : " << typeid(Type).name() << " - poolSize : " << m_poolSize << std::endl;
		// std::cout << "ObjectPool::GetMemory() : " << typeid(Type).name() << " - usedSize : " << m_usedSize << std::endl;
#endif

		return memory;
	}

	static void Push(Type* obj)
	{
		if (!obj) return;

		obj->~Type();

		{
			WriteLockGuard writeLockGuard(m_lock, typeid(ObjectPool<Type>).name());

			m_usedSize--;
			m_queue.push(obj);
		}

#if _DEBUG
		// std::cout << "ObjectPool::Push() : " << typeid(Type).name() << " - usedSize : " << m_usedSize << std::endl;
#endif
	}

	static int GetPoolSize()
	{
		WriteLockGuard writeLockGuard(m_lock, typeid(ObjectPool<Type>).name());
		return m_poolSize;
	}

	static int GetUsedSize()
	{
		WriteLockGuard writeLockGuard(m_lock, typeid(ObjectPool<Type>).name());
		return m_usedSize;
	}
};

template <typename Type>
Lock ObjectPool<Type>::m_lock;

template <typename Type>
std::queue<Type*> ObjectPool<Type>::m_queue;

template <typename Type>
int ObjectPool<Type>::m_usedSize = 0;

template <typename Type>
int ObjectPool<Type>::m_poolSize = 0;