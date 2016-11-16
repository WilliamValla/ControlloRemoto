#include "crypto_base.h"

#include <stdexcept>
#include <Windows.h>
#include <Wincrypt.h>

uintptr_t crlib::crypto_base::m_crypto_provider = 0;
crlib::crypto_base::crypto_initializer crlib::crypto_base::m_initializer;

uintptr_t crlib::crypto_base::create_hasher(HASH_TYPE type)
{
	ALG_ID id;

	switch (type)
	{
	case HASH_TYPE::MD5:
		id = CALG_MD5;
		break;

	case HASH_TYPE::SHA_160:
		id = CALG_SHA1;
		break;

	case HASH_TYPE::SHA_256:
		id = CALG_SHA_256;
		break;

	case HASH_TYPE::SHA_384:
		id = CALG_SHA_384;
		break;

	case HASH_TYPE::SHA_512:
		id = CALG_SHA_512;
		break;

	default:
		throw std::runtime_error("Invalid Hashing algo");
	}

	uintptr_t result = 0;

	if (!CryptCreateHash(m_crypto_provider, id, 0, 0, reinterpret_cast<HCRYPTHASH*>(&result)))
	{
		throw std::runtime_error("Failed to create hasher");
	}

	return result;
}

void crlib::crypto_base::free_hasher(uintptr_t hasher)
{
	CryptDestroyHash(hasher);
}

void crlib::crypto_base::hash_data(uintptr_t hasher, const uint8_t* buffer, size_t buffer_size)
{
	if (!CryptHashData(hasher, buffer, buffer_size, 0))
	{
		throw std::runtime_error("Failed to hash data");
	}
}

void crlib::crypto_base::read_hashed_data(uintptr_t hasher, HASH_TYPE type, uint8_t* buffer)
{
	DWORD hash_size = static_cast<DWORD>(type);

	if (!CryptGetHashParam(hasher, HP_HASHVAL, buffer, &hash_size, 0))
	{
		throw std::runtime_error("Failed retrieve hashed data");
	}
}

void crlib::crypto_base::gen_random(uint8_t* buffer, size_t size)
{
	if (!CryptGenRandom(m_crypto_provider, size, buffer))
	{
		throw std::runtime_error("CryptoGenRandom failed");
	}
}

crlib::crypto_base::crypto_initializer::crypto_initializer()
{
	if (!CryptAcquireContext(reinterpret_cast<HCRYPTPROV*>(&crypto_base::m_crypto_provider), NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
	{
		throw std::runtime_error("Failed to initialize CryptoAPI");
	}
}

crlib::crypto_base::crypto_initializer::~crypto_initializer()
{
	CryptReleaseContext(crypto_base::m_crypto_provider, 0);
}
