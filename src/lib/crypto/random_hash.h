#pragma once

#include <cstdint>
#include <string>
#include "crypto_base.h"
#include "..\utils\random.h"

namespace crlib
{

/*
Creates a random hash of a given size.
*/
template<size_t Size>
class random_hash
{
public:
	//Initializes the RNG, doesn't compute the hash.
	random_hash();

	random_hash(const random_hash<Size>&) = delete;
	random_hash(random_hash<Size>&&) = delete;
	random_hash<Size>& operator=(random_hash<Size>) = delete;

	//Computes the hash. Can be called multiple times.
	void generate();

	uint8_t* buffer() { return m_buffer; }

	//Returns a string version of the hash. Must call generate() before.
	std::string to_string();

	static constexpr size_t size() { return Size; }

	//Quick helper function that returns a random hashed string
	static std::string get();

private:
	uint8_t m_buffer[Size];
};


template<size_t Size>
inline random_hash<Size>::random_hash() :
	m_buffer()
{
}

template<size_t Size>
inline void random_hash<Size>::generate()
{
	crypto_base::gen_random(m_buffer, Size);
}

template<size_t Size>
inline std::string random_hash<Size>::to_string()
{
	static const char* HEX = "0123456789abcdef";

	std::string str;
	str.reserve(Size * 2);

	for (size_t i = 0; i < Size; ++i)
	{
		str.push_back(HEX[(m_buffer[i] >> 4)]);
		str.push_back(HEX[(m_buffer[i] & 0xF)]);
	}

	return str;
}

template<size_t Size>
inline std::string random_hash<Size>::get()
{
	random_hash<Size> hash;
	hash.generate();
	return hash.to_string();
}

} //namespace