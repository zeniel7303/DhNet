#pragma once

class CryptoUtil
{
public:
	// Generate PBKDF2-SHA256 hash with a random salt.
	// outHashHex: 64-char hex (32 bytes), outSaltHex: 32-char hex (16 bytes)
	static bool HashPassword(const std::string& password,
	                         std::string& outHashHex, std::string& outSaltHex);

	// Constant-time verification — prevents timing attacks
	static bool VerifyPassword(const std::string& password,
	                           const std::string& storedHashHex,
	                           const std::string& storedSaltHex);

private:
	static std::string BytesToHex(const unsigned char* data, size_t len);
	static bool        HexToBytes(const std::string& hex, std::vector<unsigned char>& out);
};
