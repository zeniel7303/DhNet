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
		std::shared_ptr<Type> sptr = { Pop(std::forward<Args>(_args)...), Push };
		return sptr;
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

		new(memory)Type(std::forward<Args>(args)...);

#if _DEBUG
		// std::cout << "ObjectPool::GetMemory() : " << typeid(Type).name() << " - poolSize : " << m_poolSize << std::endl;
		// std::cout << "ObjectPool::GetMemory() : " << typeid(Type).name() << " - usedSize : " << m_usedSize << std::endl;
#endif

		return memory;
	}

	static void Push(Type* obj)
	{
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
};

template <typename Type>
Lock ObjectPool<Type>::m_lock;

template <typename Type>
std::queue<Type*> ObjectPool<Type>::m_queue;

template <typename Type>
int ObjectPool<Type>::m_usedSize = 0;

template <typename Type>
int ObjectPool<Type>::m_poolSize = 0;

/*#pragma once
#include "pch.h"
#include <mutex>

template<typename T>
class ObjectPool
{
private:
	USE_LOCK;

	std::queue<T*>	m_queue;

	bool			m_isAlloc;
	int				m_usedSize;
	int				m_poolSize;

public:
	ObjectPool(int _size);
	~ObjectPool();

	void Init();
	void Close();

	T* NewObject();
	bool DeleteObject(T* _object);

	int GetMaxSize();
	int GetUsedSize();
};

template<typename T>
ObjectPool<T>::ObjectPool(int _size)
{
	m_isAlloc = false;
	m_usedSize = 0;
	m_poolSize = _size;
}

template<typename T>
ObjectPool<T>::~ObjectPool()
{
	Close();
}

template<typename T>
void ObjectPool<T>::Init()
{
	ASSERT_CRASH(m_isAlloc == false);

	WRITE_LOCK;

	for (int i = 0; i < m_poolSize; ++i)
	{
		try
		{
			T* tObject = new T;
			m_queue.push(tObject);
		}
		catch (std::exception&)
		{
			assert(0 && "Init Failed");
		}
	}

	m_usedSize = 0;
	m_isAlloc = true;
}

template<typename T>
void ObjectPool<T>::Close()
{
	WRITE_LOCK;

	if (m_isAlloc == false) return;

	while (m_queue.empty() == false)
	{
		m_queue.pop();
	}

	m_usedSize = 0;
	m_isAlloc = false;
}

template<typename T>
T* ObjectPool<T>::NewObject()
{
	ASSERT_CRASH(m_isAlloc == true);

	T* object = nullptr;

	{
		WRITE_LOCK;

		if (m_usedSize >= GetMaxSize()) return object;

		object = m_queue.front();
		m_queue.pop();
		m_usedSize++;
	}

	ASSERT_CRASH(object != nullptr);

	return object;
}

template<typename T>
bool ObjectPool<T>::DeleteObject(T* _object)
{
	ASSERT_CRASH(m_isAlloc == true);
	ASSERT_CRASH(_object != nullptr);

	{
		WRITE_LOCK;

		m_queue.push(_object);
		m_usedSize--;
	}

	return true;
}

template<typename T>
int ObjectPool<T>::GetMaxSize()
{
	return m_poolSize;
}

template<typename T>
int ObjectPool<T>::GetUsedSize()
{
	return m_usedSize;
}*/