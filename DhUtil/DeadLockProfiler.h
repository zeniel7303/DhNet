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
	// ��尡 �߰ߵ� ���Ÿ� ����ϴ� �迭
	std::vector<int32>						m_discoveredOrder;
	// ��尡 �߰ߵ� ����
	int32									m_discoveredCount;
	// DFS(i)�� ���� �Ǿ����� ����
	std::vector<bool>						m_finished;
	std::vector<int32>						m_parent;

public:
	void PushLock(const char* _name);
	void PopLock(const char* _name);
	void CheckCycle();

private:
	void DFS(int32 _here);
};