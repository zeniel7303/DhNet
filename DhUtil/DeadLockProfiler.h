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
	// 노드가 발견된 순거를 기록하는 배열
	vector<int32>						m_discoveredOrder;
	// 노드가 발견된 순서
	int32								m_discoveredCount;
	// DFS(i)가 종료 되었는지 여부
	vector<bool>						m_finished;
	vector<int32>						m_parent;

public:
	void PushLock(const char* _name);
	void PopLock(const char* _name);
	void CheckCycle();

private:
	void DFS(int32 _here);
};

// 전역 DeadLockProfiler 인스턴스 선언
extern DeadLockProfiler GDeadLockProfiler;