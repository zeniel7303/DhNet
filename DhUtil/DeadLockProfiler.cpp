#include "pch.h"
#include "DeadLockProfiler.h"

/*----------------------------
		DeadLockProfiler
------------------------------*/

void DeadLockProfiler::PushLock(const char* _name)
{
	lock_guard<std::mutex> guard(m_lock);

	// ���̵� ã�ų� �߱��Ѵ�.
	int32 lockId = 0;

	auto findIt = m_nameToId.find(_name);
	if (findIt == m_nameToId.end())
	{
		lockId = static_cast<int32>(m_nameToId.size());
		m_nameToId[_name] = lockId;
		m_idToName[lockId] = _name;
	}
	else
	{
		lockId = findIt->second;
	}

	// ��� �ִ� ���� �־��ٸ�
	if (LLockStack.empty() == false)
	{
		// ������ �߰ߵ��� ���� ���̽���� ����� ���� �ٽ� Ȯ���Ѵ�. (����Ŭ �Ǻ�)
		const int32 prevId = LLockStack.top();
		if (lockId != prevId)
		{
			set<int32>& history = m_lockHistory[prevId];
			if (history.find(lockId) == history.end())
			{
				history.insert(lockId);
				CheckCycle();
			}
		}
	}

	LLockStack.push(lockId);
}

void DeadLockProfiler::PopLock(const char* _name)
{
	lock_guard<std::mutex> guard(m_lock);

	// ���� ��� �Ǵ� ���� �������� �ڵ��
	{
		if (LLockStack.empty()) CRASH("MULTIPLE_UNLOCK");

		int32 lockId = m_nameToId[_name];
		if (LLockStack.top() != lockId) CRASH("INVALID_UNLOCK");
	}

	LLockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(m_nameToId.size());
	m_discoveredOrder = vector<int32>(lockCount, -1);
	m_discoveredCount = 0;
	m_finished = vector<bool>(lockCount, false);
	m_parent = vector<int32>(lockCount, -1);

	for (int32 lockId = 0; lockId < lockCount; lockId++)
		DFS(lockId);

	// ������ �������� �����Ѵ�.
	m_discoveredOrder.clear();
	m_finished.clear();
	m_parent.clear();
}

void DeadLockProfiler::DFS(int32 _here)
{
	if (m_discoveredOrder[_here] != -1)
		return;

	m_discoveredOrder[_here] = m_discoveredCount++;

	// ��� ������ ������ ��ȸ�Ѵ�.
	auto findIt = m_lockHistory.find(_here);
	if (findIt == m_lockHistory.end())
	{
		m_finished[_here] = true;
		return;
	}

	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		// ���� �湮�� ���� ���ٸ� �湮�Ѵ�.
		if (m_discoveredOrder[there] == -1)
		{
			m_parent[there] = _here;
			DFS(there);
			continue;
		}

		// _here�� there���� ���� �߰ߵǾ��ٸ�, there�� _here�� �ļ��̴�. (������ ����)
		if (m_discoveredOrder[_here] < m_discoveredOrder[there])
			continue;

		// �������� �ƴϴ�.
		// DFS(there)�� ���� �������� �ʾҴٸ�, there�� _here�� �����̴�. (������ ����)
		if (m_finished[there] == false)
		{
			printf("%s -> %s \n", m_idToName[_here], m_idToName[there]);

			int32 now = _here;
			while (true)
			{
				printf("%s -> %s \n", m_idToName[m_parent[now]], m_idToName[now]);
				now = m_parent[now];

				if (now == there) break;
			}

			CRASH("DEADLOCK_DETECTED");
		}
	}

	m_finished[_here] = true;
}