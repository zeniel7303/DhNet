#pragma once
#include "pch.h"

template<typename T>
class ObjectPool
{
private:
	USE_LOCK;

	std::queue<T*>	m_queue;
	std::vector<T*>	m_vector;

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

	m_vector.reserve(_size);
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
			m_vector.push_back(tObject);
			m_queue.push(tObject);
		}
		catch (std::exception& _exception)
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

	for (auto& _object : m_vector)
	{
		static_assert(std::is_pointer<T*>::value);

		delete[] _object;
		_object = nullptr;
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

	bool flag = false;
	for (const auto& _auto : m_vector)
	{
		if (_auto == _object)
		{
			flag = true;
			break;
		}
	}

	if (flag == false)
	{
		return false;
	}

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
}