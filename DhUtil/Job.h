#pragma once
#include <functional>
#include <iostream>

using CallbackType = std::function<void()>;

class Job
{
private:
	CallbackType m_callback;

public:
	Job() 
	{
		// std::cout << "Constructor 1" << std::endl; 
	}

	Job(CallbackType&& _callback) : m_callback(std::move(_callback)) 
	{
		// std::cout << "Constructor 2" << std::endl;
	}

	template<typename T, typename Ret, typename... Args>
	Job(std::shared_ptr<T> _owner, Ret(T::* _memFunc)(Args...), Args&&... _args)
	{
		m_callback = [_owner, _memFunc, _args...]()
			{
				(_owner.get()->*_memFunc)(_args...);
			};

		// std::cout << "Constructor 3" << std::endl;
	}

	void Execute()
	{
		try
		{
			if (m_callback)
				m_callback();
		}
		catch (const std::exception& e)
		{
			// 예외 처리 로직 (로그 출력 등)
			std::cerr << "Job execution failed: " << e.what() << std::endl;
		}
		catch (...)
		{
			// 알 수 없는 예외 처리
			std::cerr << "Job execution failed: Unknown error" << std::endl;
		}
	}
};