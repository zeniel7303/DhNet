#pragma once
#include "pch.h"

template<typename T>
class Handler
{
public:
	std::map<int, T> m_packetList;

public:
	Handler() {}
	~Handler() {}
	bool Register(int _packetNum, T&& _type);
	template<typename... arg>
	bool Process(int _packetNum, arg... _arg);
};

template<typename T>
inline bool Handler<T>::Register(int _packetNum, T&& _type)
{
	auto result = m_packetList.try_emplace(_packetNum, _type);

	if (result.second == NULL) return false;

	return true;
}

template<typename T>
template<typename ...arg>
inline bool Handler<T>::Process(int _packetNum, arg ..._arg)
{
	auto value = m_packetList.find(_packetNum);

	if (value == m_packetList.end()) return false;

	auto result = value->second(_arg...);
	return result;
}