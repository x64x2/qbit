#ifndef HASH_HPP
#define HASH_HPP

#include <boost/cstdint.hpp>
#include "qbit/peer.hpp"
#include "qbit/assert.hpp"

#ifdef GCRYPT
#include <gcrypt.h>

#elif TORRENT_USE_COMMONCRYPTO
#elif defined OPENSSL
extern "C"
{
#include <openssl/sha.h>
}
namespace qbit
{

	struct sha_ctx
	{
		boost::uint32_t state[5];
		boost::uint32_t count[2];
		boost::uint8_t buffer[64];
	};

	void SHA1_init(sha_ctx* context);
	void SHA1_update(sha_ctx* context, boost::uint8_t const* data, boost::uint32_t len);
	void SHA1_final(boost::uint8_t* digest, sha_ctx* context);
} 

namespace qbit
{
	class hash
	{
	public:
		hash();
		hash(const char* data, int len);

#ifdef GCRYPT
		hash(hash const& h);
		hash& operator=(hash const& h);
	
		hash& update(std::string const& data) { update(data.c_str(), int(data.size())); return *this; }
		hash& update(const char* data, int len);
	
		sha1_hash final();
		void reset();

#ifdef GCRYPT
		~hash();

	private:

#ifdef GCRYPT
		gcry_md_hd_t;
	};
}
