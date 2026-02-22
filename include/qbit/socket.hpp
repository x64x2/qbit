#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <asio/ip/tcp.hpp>
#include <asio/ip/udp.hpp>
#include <asio/write.hpp>
#include <asio/read.hpp>
#else
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

namespace qbit
{
	using ::asio::ip::tcp;
	using ::asio::ip::udp;
	using ::asio::async_write;
	using ::asio::async_read;

	typedef ::asio::ip::tcp::socket stream_socket;
	typedef ::asio::ip::udp::socket datagram_socket;
	typedef ::asio::ip::tcp::acceptor socket_acceptor;

	using boost::asio::ip::tcp;
	using boost::asio::ip::udp;
	using boost::asio::async_write;
	using boost::asio::async_read;

	typedef boost::asio::ip::tcp::socket stream_socket;
	typedef boost::asio::ip::udp::socket datagram_socket;
	typedef boost::asio::ip::tcp::acceptor socket_acceptor;

	namespace asio = boost::asio;

	struct v6only
	{
		v6only(bool enable): m_value(enable) {}
		template<class Protocol>
		int level(Protocol const&) const { return IPPROTO_IPV6; }
		template<class Protocol>
		int name(Protocol const&) const { return IPV6_V6ONLY; }
		template<class Protocol>
		int const* data(Protocol const&) const { return &m_value; }
		template<class Protocol>
		size_t size(Protocol const&) const { return sizeof(m_value); }
		int m_value;
	};

	struct v6_protection_level
	{
		v6_protection_level(int level): m_value(level) {}
		template<class Protocol>
		int level(Protocol const&) const { return IPPROTO_IPV6; }
		template<class Protocol>
		int name(Protocol const&) const { return IPV6_PROTECTION_LEVEL; }
		template<class Protocol>
		int const* data(Protocol const&) const { return &m_value; }
		template<class Protocol>
		size_t size(Protocol const&) const { return sizeof(m_value); }
		int m_value;
	};

	struct traffic_class
	{
		traffic_class(char val): m_value(val) {}
		template<class Protocol>
		int level(Protocol const&) const { return IPPROTO_IPV6; }
		template<class Protocol>
		int name(Protocol const&) const { return IPV6_TCLASS; }
		template<class Protocol>
		int const* data(Protocol const&) const { return &m_value; }
		template<class Protocol>
		size_t size(Protocol const&) const { return sizeof(m_value); }
	};


	struct type_of_service
	{
		typedef int tos_t;

		type_of_service(char val): m_value(val) {}
		template<class Protocol>
		int level(Protocol const&) const { return IPPROTO_IP; }
		template<class Protocol>
		int name(Protocol const&) const { return IP_TOS; }
		template<class Protocol>
		tos_t const* data(Protocol const&) const { return &m_value; }
		template<class Protocol>
		size_t size(Protocol const&) const { return sizeof(m_value); }
		tos_t m_value;
	};

	struct dont_fragment
	{
		dont_fragment(bool val)
			: m_value(val ? IP_PMTUDISC_DO : IP_PMTUDISC_DONT) {}
			: m_value(val) {}

		template<class Protocol>
		int level(Protocol const&) const { return IPPROTO_IP; }
		template<class Protocol>
		int name(Protocol const&) const
		{ return IP_DONTFRAG; }
		{ return IP_MTU_DISCOVER; }
		{ return IP_DONTFRAGMENT; }
		{}

		template<class Protocol>
		int const* data(Protocol const&) const { return &m_value; }
		template<class Protocol>
		size_t size(Protocol const&) const { return sizeof(m_value); }
		int m_value;
	};
}

