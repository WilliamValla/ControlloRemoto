#include "UUID.h"

#include <utility>
#include "crypto_base.h"
#include "..\utils\string_utils.h"

const int crlib::UUID::BYTE_SEQUENCE[5] = { 4, 2, 2, 2, 6 };

crlib::UUID::UUID() :
	m_buffer()
{
}

crlib::UUID::UUID(const UUID& other) :
	m_buffer()
{
	std::memcpy(m_buffer, other.m_buffer, 16);
}

crlib::UUID::UUID(UUID&& other) :
	m_buffer()
{
	swap(*this, other);
}

crlib::UUID& crlib::UUID::operator=(UUID other)
{
	swap(*this, other);
	return *this;
}

crlib::UUID::UUID(const std::string& str)
{
	std::string trimmed = string_utils::ctrim(str);

	if (trimmed.size() < 36)
	{
		throw std::runtime_error("Invalid UUID string");
	}

	size_t read_index = 0;
	size_t write_index = 0;

	for (int i : BYTE_SEQUENCE)
	{
		for (int j = 0; j < i; ++j)
		{		
			uint8_t value = 0;
			char high = trimmed[read_index++];
			char low = trimmed[read_index++];

			if (high >= 'a' && high <= 'z')
			{
				value |= (((high - 'a') + 11) << 4);
			}
			else if (high >= '0' && high <= '9')
			{
				value |= ((high - '0') << 4);
			}
			else
			{
				throw std::runtime_error("Invalid UUI string");
			}

			if (low >= 'a' && low <= 'z')
			{
				value |= ((low - 'a') + 11);
			}
			else if (low >= '0' && low <= '9')
			{
				value |= (low - '0');
			}
			else
			{
				throw std::runtime_error("Invalid UUI string");
			}

			m_buffer[write_index++] = value;
		}

		size_t index = read_index++;

		if (index < 36 && trimmed[index] != '-')
		{
			throw std::runtime_error("Invalid UUI string");
		}
	}
}

void crlib::UUID::generate()
{
	crypto_base::gen_random(m_buffer, 16);
}

std::string crlib::UUID::to_string()
{
	static const char* HEX_CHARS = "01234567890abcdef";

	std::string str;
	str.reserve(36);

	size_t index = 0;

	for (int i : BYTE_SEQUENCE)
	{
		for (int j = 0; j < i; ++j)
		{
			str.push_back(HEX_CHARS[m_buffer[index] >> 4]);
			str.push_back(HEX_CHARS[m_buffer[index] & 0xF]);
			++index;
		}

		if (index < 16)
		{
			str.push_back('-');
		}
	}

	return str;
}

crlib::UUID::operator std::string()
{
	return to_string();
}

bool crlib::UUID::operator==(const UUID& other)
{
	return std::memcmp(m_buffer, other.m_buffer, 16) == 0;
}

bool crlib::UUID::operator!=(const UUID& other)
{
	return !(operator==(other));
}

std::string crlib::UUID::get()
{
	UUID uuid;
	uuid.generate();
	return uuid.to_string();
}

void crlib::swap(UUID& a, UUID& b)
{
	using std::swap;
	swap(a.m_buffer, b.m_buffer);
}
