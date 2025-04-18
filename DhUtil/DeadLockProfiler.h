#pragma once
#include <map>
#include <vector>
#include <mutex>
#include <set>
#include <unordered_map>

class DeadLockProfiler
{
private:
	std::mutex								m_lock;

	std::unordered_map<const char*, int32>	m_nameToId;
	std::unordered_map<int32, const char*>	m_idToName;
	std::map<int32, std::set<int32>>		m_lockHistory;

private:
	// 노드가 발견된 순거를 기록하는 배열
	std::vector<int32>						m_discoveredOrder;
	// 노드가 발견된 순서
	int32									m_discoveredCount;
	// DFS(i)가 종료 되었는지 여부
	std::vector<bool>						m_finished;
	std::vector<int32>						m_parent;

public:
	void PushLock(const char* _name);
	void PopLock(const char* _name);
	void CheckCycle();

private:
	void DFS(int32 _here);
};