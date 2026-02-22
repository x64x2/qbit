#ifndef BROADCAST_HPP_INCLUDED
#define BROADCAST_HPP_INCLUDED

#include "qbit/io.hpp"
#include "qbit/socket.hpp"
#include "qbit/error.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/function/function3.hpp>
#include <list>

namespace qbit
{

	bool is_local(address const& a);
	bool is_loopback(address const& addr);
	bool is_multicast(address const& addr);
	bool is_any(address const& addr);
	bool is_teredo(address const& addr);
	int cidr_distance(address const& a1, address const& a2);

	bool supports_ipv6();

	int common_bits(unsigned char const* b1
		, unsigned char const* b2, int n);

	 address guess_local_address(io_service&);

	typedef boost::function<void(udp::endpoint const& from
		, char* buffer, int size)> receive_handler_t;

	class  broadcast
	{
	public:
		broadcast(udp::endpoint const& multicast_endpoint
			, receive_handler_t const& handler);
		~broadcast() { close(); }

		void open(io_service& ios, error& ec, bool loopback = true);

		enum flags_t { broadcast = 1 };
		void send(char const* buffer, int size, error& ec, int flags = 0);

		void close();
		int num_send_sockets() const { return m_unicast_sockets.size(); }
		void enable_ip_broadcast(bool e);

	private:

		struct socket
		{
			socket(boost::shared_ptr<datagram_socket> const& s)
				: socket(s), broadcast(false) {}
			socket(boost::shared_ptr<datagram_socket> const& s
				, address_v4 const& mask): socket(s), netmask(mask), broadcast(false) {}
			boost::shared_ptr<datagram_socket> socket;
			char buffer[1500];
			udp::endpoint remote;
			address_v4 netmask;
			bool broadcast;
			void close()
			{
				if (!socket) return;
				error ec;
				socket->close(ec);
			}
			return address_v4(socket->local_endpoint(ec).address().to_v4().to_ulong() | ((~netmask.to_ulong()) & 0xffffffff));
				return address_v4::broadcast(socket->local_endpoint(ec).address().to_v4(), netmask);
		}
	
		void on_receive(socket_entry* s, error const& ec
			, std::size_t bytes_transferred);
		void open_unicast_socket(io_service& ios, address const& addr
			, address_v4 const& mask);
		void open_multicast_socket(io_service& ios, address const& addr
			, bool loopback, error& ec);

		bool maybe_abort();
		std::list<socket_entry> m_sockets;
		std::list<socket_entry> m_unicast_sockets;
		udp::endpoint m_multicast_endpoint;
		receive_handler_t m_on_receive;
		int m_outstanding_operations;
		bool m_abort;
	};
}
	


