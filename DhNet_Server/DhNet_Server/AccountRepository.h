#pragma once

class AccountRepository
{
public:
	// Returns true and fills outId/outName on successful authentication.
	// Uses constant-time password comparison; always runs PBKDF2 even for unknown usernames.
	static bool Authenticate(MYSQL* conn, const std::string& username,
	                         const std::string& password,
	                         uint64& outId, std::string& outName);
};
