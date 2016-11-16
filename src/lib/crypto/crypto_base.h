#pragma once

#include <cstdint>

namespace crlib
{

/*
Enumeration for the hashing algorithms.
The vaule of the enum is the size of the hash result in bytes.
The size of the string is 2 times that value.
*/
enum class HASH_TYPE : uint8_t
{
	MD5 = 16,
	SHA_160 = 20,
	SHA_256 = 32,
	SHA_384 = 48,
	SHA_512 = 64
};

/*
Abstraction class for the CryptoApi functions.
Don't use directly.
*/
class crypto_base
{

private:
	struct crypto_initializer
	{
		crypto_initializer();
		~crypto_initializer();
	};

public:
	crypto_base() = delete;

	static uintptr_t create_hasher(HASH_TYPE type);
	static void free_hasher(uintptr_t hasher);

	static void hash_data(uintptr_t hasher, const uint8_t* buffer, size_t buffer_size);
	static void read_hashed_data(uintptr_t hasher, HASH_TYPE type, uint8_t* buffer);

	static void gen_random(uint8_t* buffer, size_t size);

private:
	static uintptr_t m_crypto_provider;
	static crypto_initializer m_initializer;
};


} //namespace