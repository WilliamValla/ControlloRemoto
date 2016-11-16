#pragma once

#include <string>
#include "crypto_base.h"

namespace crlib
{

/*
Class used for hashing data (either strings or raw bytes).
Once the hash function is computed, data CANNOT be added to it.
*/
template<HASH_TYPE Type>
class hash
{
public:
	hash();
	hash(uint8_t* buffer);
	~hash();

	hash(const hash<Type>&) = delete;
	hash(hash<Type>&&) = delete;
	hash<Type>& operator=(hash<Type>) = delete;

	//Add raw bytes to the hasher
	void add(const uint8_t* buffer, size_t buffer_size);

	//Add a string to the hasher
	void add(const std::string& str);

	//The return pointer must be copied before the hasher is destroyed
	//After this function is called, data cannot be added to the hasher
	const uint8_t* compute_hash();
	//After this function is called, data cannot be added to the hasher
	std::string compute_hash_string();	

	static constexpr size_t size() { return static_cast<size_t>(Type); }

	//Helper function to return the hashed version of the passed string
	static std::string hash_string(const std::string& str);

private:
	uintptr_t m_id;
	uint8_t m_buffer[size()];
};


template<HASH_TYPE Type>
inline hash<Type>::hash() :
	m_id(crypto_base::create_hasher(Type)),
	m_buffer()
{
}

template<HASH_TYPE Type>
inline hash<Type>::hash(uint8_t* buffer) :
	m_id(0),
	m_buffer()
{
	std::memcpy(m_buffer, buffer, size());
}

template<HASH_TYPE Type>
inline hash<Type>::~hash()
{
	crypto_base::free_hasher(m_id);
}

template<HASH_TYPE Type>
inline void hash<Type>::add(const uint8_t* buffer, size_t buffer_size)
{
	crypto_base::hash_data(m_id, buffer, buffer_size);
}

template<HASH_TYPE Type>
inline void hash<Type>::add(const std::string& str)
{
	crypto_base::hash_data(m_id, reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

template<HASH_TYPE Type>
inline const uint8_t* hash<Type>::compute_hash()
{
	crypto_base::read_hashed_data(m_id, Type, m_buffer);
	return m_buffer;
}

template<HASH_TYPE Type>
inline std::string hash<Type>::compute_hash_string()
{
	static const char* HEX = "0123456789abcdef";

	crypto_base::read_hashed_data(m_id, Type, m_buffer);
	std::string str;
	str.reserve(size() * 2);

	for (size_t i = 0; i < size(); ++i)
	{
		str.push_back(HEX[(m_buffer[i] >> 4)]);
		str.push_back(HEX[(m_buffer[i] & 0xF)]);
	}

	return str;
}

template<HASH_TYPE Type>
inline std::string hash<Type>::hash_string(const std::string & str)
{
	hash<Type> hasher;
	hasher.add(str);
	return hasher.compute_hash_string();
}

} //namespace