#pragma once
#include <map>
#include <vector>
#include <mutex>
#include <set>
#include <unordered_map>

/*----------------------------
		DeadLockProfiler
------------------------------*/

class DeadLockProfiler
{
private:
	mutex								m_lock;

	unordered_map<const char*, int32>	m_nameToId;
	unordered_map<int32, const char*>	m_idToName;
	map<int32, set<int32>>				m_lockHistory;

private:
	// ��尡 �߰ߵ� ���Ÿ� ����ϴ� �迭
	vector<int32>						m_discoveredOrder;
	// ��尡 �߰ߵ� ����
	int32								m_discoveredCount;
	// DFS(i)�� ���� �Ǿ����� ����
	vector<bool>						m_finished;
	vector<int32>						m_parent;

public:
	void PushLock(const char* _name);
	void PopLock(const char* _name);
	void CheckCycle();

private:
	void DFS(int32 _here);
};

// ���� DeadLockProfiler �ν��Ͻ� ����
extern DeadLockProfiler GDeadLockProfiler;