#pragma once

template<typename T>
class LockQueue
{
private:
	USE_LOCK
	std::queue<T> _items;

public:
	void Push(T item)
	{
		WRITE_LOCK
		_items.push(item);
	}

	T Pop()
	{
		WRITE_LOCK
		if (_items.empty())
			return T();

		T ret = _items.front();
		_items.pop();
		return ret;
	}

	void PopAll(OUT std::vector<T>& items)
	{
		WRITE_LOCK
		while (T item = Pop())
			items.push_back(item);
	}

	void Clear()
	{
		WRITE_LOCK
		_items = std::queue<T>();
	}
};