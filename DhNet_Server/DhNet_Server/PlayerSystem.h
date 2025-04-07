#pragma once

class Player;

class PlayerSystem
{
private:
	USE_LOCK;
	concurrency::concurrent_unordered_map<uint64, std::shared_ptr<Player>> m_players;

public:
	PlayerSystem() = default;
	~PlayerSystem();
	
	void Add(std::shared_ptr<Player> _player);
	void Remove(std::shared_ptr<Player> _player);
	std::shared_ptr<Player> Find(int _id);
};