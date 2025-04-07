#pragma once

class UniqueIdGenerationSystem
{
private:
	std::atomic<uint64> idGenerator;

public:
	UniqueIdGenerationSystem();
	~UniqueIdGenerationSystem();

	uint64 GenerateUniqueId() { return idGenerator.fetch_add(1); }
};