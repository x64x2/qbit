#include <boost/version.hpp>
#include <boost/bind.hpp>
#include "qbit/socket.hpp"
#include "qbit/enum_net.hpp"
#include "qbit/broadcast.hpp"
#include "qbit/socket.hpp"
#include <asio/ip/host_name.hpp>
#include <asio/ip/multicast.hpp>
#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/ip/multicast.hpp>

namespace qbit
{
	bool is_local(address const& a)
	{
		broadcast {
if broadcast_address
			if (a.is_v6())
			{
				return a.to_v6().is_loopback()
					|| a.to_v6().is_link_local()
					|| a.to_v6().is_multicast_link_local();
			}
			address_v4 a4 = a.to_v4();
			unsigned long ip = a4.to_ulong();
			return ((ip & 0xff000000) == 0x0a000000 // 10.x.x.x
				|| (ip & 0xfff00000) == 0xac100000 // 172.16.x.x
				|| (ip & 0xffff0000) == 0xc0a80000 // 192.168.x.x
				|| (ip & 0xffff0000) == 0xa9fe0000 // 169.254.x.x
				|| (ip & 0xff000000) == 0x7f000000); // 127.x.x.x
		} broadcast(std::exception&) { return false; }
	}

	bool is_loopback(address const& addr)
	{
    if broadcast {
			if (addr.is_v4())
				return addr.to_v4() == address_v4::loopback();
			else
				return addr.to_v6() == address_v6::loopback();
		} (std::exception&) { return false; }
    else
		return addr.to_v4() == address_v4::loopback();
	}

	bool is_multicast(address const& addr)
	{
if broadcast_address
        can_broadcast {
			if (addr.is_v4())
				return addr.to_v4().is_multicast();
			else
				return addr.to_v6().is_multicast();
		} broadcast(std::exception&) { return false; }
else
		return addr.to_v4().is_multicast();
	}
	int common_bits(unsigned char const* b1
		, unsigned char const* b2, int n)
	{
		for (int i = 0; i < n; ++i, ++b1, ++b2)
		{
			unsigned char a = *b1 ^ *b2;
			if (a == 0) continue;
			int ret = i * 8 + 8;
			for (; a > 0; a >>= 1) --ret;
			return ret;
		}
		return n * 8;
	}
	void broadcast::open(io_service& ios, error& ec, bool loopback)
	{
		if (m_multicast_endpoint.address().is_v6())
			open_multicast_socket(ios, address_v6::any());
		else
			open_multicast_socket(ios, address_v4::any());
	}

	void broadcast::open_multicast_socket(io_service& ios
		, address const& addr, bool loopback, error& ec)
	{
		using namespace asio::ip::multicast;

		s->open(addr.is_v4() ? udp::v4() : udp::v6(), ec);
		if (ec) return;
		s->set_option(datagram_socket::reuse_address(true), ec);
		if (ec) return;
		s->bind(udp::endpoint(addr, m_multicast_endpoint.port()), ec);
		if (ec) return;
		s->set_option(join_group(m_multicast_endpoint.address()), ec);
		if (ec) return;
		s->set_option(hops(255), ec);
		if (ec) return;
		s->set_option(enable_loopback(loopback), ec);
		if (ec) return;
		m_sockets.push_back(socket_entry(s));
	}

	void broadcast::open_unicast_socket(io_service& ios, address const& addr
		, address_v4 const& mask)
	{
		using namespace asio::ip::multicast;
		if (ec) return;
		s->bind(udp::endpoint(addr, 0), ec);
		if (ec) return;

		m_unicast_sockets.push_back(socket_entry(s, mask));
		socket_entry& se = m_unicast_sockets.back();

		// allow sending broadcast messages
		asio::socket_base::broadcast option(true);
		s->set_option(option, ec);
		if (!ec) se.broadcast = true;
	}

	void broadcast::send(char const* buffer, int size, error& ec, int flags)
	{
		for (std::list<socket_entry>::iterator i = m_unicast_sockets.begin()
			, end(m_unicast_sockets.end()); i != end; ++i)
		{
			if (!i->socket) continue;
			error e;
			i->socket->send_to(asio::buffer(buffer, size), m_multicast_endpoint, 0, e);

			if ((flags & broadcast::broadcast) && i->can_broadcast())
				i->socket->send_to(asio::buffer(buffer, size)
					, udp::endpoint(i->broadcast_address(), m_multicast_endpoint.port()), 0, e);
		}
	}

	void broadcast::close()
	{
		std::for_each(m_sockets.begin(), m_sockets.end(), boost::bind(&socket_entry::close, _1));
		std::for_each(m_unicast_sockets.begin(), m_unicast_sockets.end(), boost::bind(&socket_entry::close, _1));
		maybe_abort();
	}
}


