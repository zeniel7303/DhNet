#pragma once
#include "GameSession.h"
#include "Player.h"
#include "Job.h"

class Room
{
	friend class EnterJob;
	friend class LeaveJob;
	friend class BroadcastJob;

private:
	map<uint64, shared_ptr<Player>> m_players;

	JobQueue m_jobs;

	// ���� ���� �Ұ����ϵ��� private
private:
	// �̱۾����� ȯ������ ����
	void Enter(shared_ptr<Player> _player);
	void Leave(shared_ptr<Player> _player);
	void Broadcast(shared_ptr<Sender> _sender);

public:
	// ��Ƽ������ ȯ�濡���� job���� ���ٽ��Ѿ���
	void PushJob(shared_ptr<Job> job) { m_jobs.Push(job); }
	void FlushJob();
};

extern Room GRoom;

// Room Jobs
class EnterJob : public Job
{
public:
	Room& m_room;
	shared_ptr<Player> m_player;

public:
	EnterJob(Room& _room, shared_ptr<Player> _player) : m_room(_room), m_player(_player)
	{
	}

	virtual void Execute() override
	{
		m_room.Enter(m_player);
	}
};

class LeaveJob : public Job
{
public:
	Room& m_room;
	shared_ptr<Player> m_player;

public:
	LeaveJob(Room& _room, shared_ptr<Player> _player) : m_room(_room), m_player(_player)
	{
	}

	virtual void Execute() override
	{
		m_room.Leave(m_player);
	}
};

class BroadcastJob : public Job
{
public:
	Room& m_room;
	shared_ptr<Sender> m_sendBuffer;

public:
	BroadcastJob(Room& _room, shared_ptr<Sender> _sendBuffer) : m_room(_room), m_sendBuffer(_sendBuffer)
	{
	}

	virtual void Execute() override
	{
		m_room.Broadcast(m_sendBuffer);
	}
};