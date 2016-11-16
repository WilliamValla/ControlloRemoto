#pragma once

#include <cstdint>
#include <string>

namespace crlib
{

class UUID
{
public:
	UUID();
	UUID(const std::string& str);
	UUID(const UUID& other);
	UUID(UUID&& other);

	UUID& operator=(UUID other);

	void generate();

	std::string to_string();

	operator std::string();

	bool operator==(const UUID& other);
	bool operator!=(const UUID& other);

	static std::string get();

	static constexpr size_t size_bytes() { return 16; }
	static constexpr size_t size_string() { return 36; }

	friend void swap(UUID& a, UUID& b);

private:
	uint8_t m_buffer[16];
	static const int BYTE_SEQUENCE[5];
};

} //namespace