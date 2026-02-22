#ifndef UDP_HPP
#define UDP_HPP


#include "qbit/socket.hpp"
#include "qbit/io.hpp"
#include "qbit/error.hpp"
#include "qbit/session.hpp"
#include "qbit/buffer.hpp"
#include "qbit/thread.hpp"
#include "qbit/time.hpp"

#include <deque>

namespace qbit
{
	struct udp_observer
	{
		virtual bool incoming_packet(error const& ec
			, udp::endpoint const&, char const* buf, int size) = 0;
		virtual bool incoming_packet(error const& ec
			, char const* hostname, char const* buf, int size) { return false; }
		virtual void writable() {}
		virtual void socket_drained() {}
	};

	class udp
	{
	public:
		udp(io_service& ios, connection_queue& cc);
		~udp();

		enum flags_t { dont_drop = 1, peer_connection = 2, dont_queue = 4 };

		bool is_open() const
		{
			return m_ipv4_sock.is_open()
				|| m_ipv6_sock.is_open()

				;
		}
		io_service& get_io_service() { return m_ipv4_sock.get_io_service(); }

		void subscribe(udp_observer* o);
		void unsubscribe(udp_observer* o);
		void send_hostname(char const* hostname, int port, char const* p
			, int len, error& ec, int flags = 0);

		void send(udp::endpoint const& ep, char const* p, int len
			, error& ec, int flags = 0);
		void bind(udp::endpoint const& ep, error& ec);
		void close();
		int local_port() const { return m_bind_port; }

		void set_proxy_settings(proxy_settings const& ps);
		proxy_settings const& get_proxy_settings() { return m_proxy_settings; }
		void set_force_proxy(bool f) { m_force_proxy = f; }

		bool is_closed() const { return m_abort; }
		tcp::endpoint local_endpoint(error& ec) const
		{
			udp::endpoint ep = m_ipv4_sock.local_endpoint(ec);
			return tcp::endpoint(ep.address(), ep.port());
		}

		void set_buf_size(int s);

		template <class SocketOption>
		void get_option(SocketOption const& opt, error& ec)
		{
			m_ipv4_sock.get_option(opt, ec);
			m_ipv6_sock.get_option(opt, ec);
		}
		template <class SocketOption>
		void set_option(SocketOption const& opt, error& ec)
		{
			m_ipv4_sock.set_option(opt, ec);
			m_ipv6_sock.set_option(opt, ec);
		}

		template <class SocketOption>
		void get_option(SocketOption& opt, error& ec)
		{
			m_ipv4_sock.get_option(opt, ec);
		}

		udp::endpoint proxy_addr() const { return m_proxy_addr; }

	protected:

		struct queued_packet
		{
			udp::endpoint ep;
			char* hostname;
			buffer buf;
			int flags;
		};
		int num_outstanding() const
		{
			return m_v4_outstandin + m_v6_outstanding;
		}
	private:
		udp(udp const&);
		udp& operator=(udp const&);

		void call_drained_handler();
		void call_writable_handler();

		void on_writable(error const& ec, udp::socket* s);

		void setup_read(udp::socket* s);
		void on_read(error const& ec, udp::socket* s);
		void on_read_impl(udp::socket* sock, udp::endpoint const& ep
			, error const& e, std::size_t bytes_transferred);
		void on_name_lookup(error const& e, tcp::resolver::iterator i);
		void on_timeout();
		void on_connect(int ticket);
		void on_connected(error const& ec, int ticket);
		void handshake1(error const& e);
		void handshake2(error const& e);
		void handshake3(error const& e);
		void handshake4(error const& e);
		void socks_forward_udp();
		void connect1(error const& e);
		void connect2(error const& e);
		void hung_up(error const& e);

		void drain_queue();

		void wrap(udp::endpoint const& ep, char const* p, int len, error& ec);
		void wrap(char const* hostname, int port, char const* p, int len, error& ec);
		void unwrap(error const& e, char const* buf, int size);
		mutable pthread_t m_thread;

		bool is_single_thread() const
		{
			if (m_thread == 0)
				m_thread = pthread_self();
			return m_thread == pthread_self();
			return true;
		}
		udp::socket m_ipv4_sock;
		int m_buf_size;
		int m_new_buf_size;
		char* m_buf;
		udp::socket m_ipv6_sock;

		boost::uint16_t m_bind_port;
		boost::uint8_t m_v4_outstanding;
		boost::uint8_t m_v6_outstanding;

		tcp::socket m_socks5_sock;
		int m_connection_ticket;
		proxy_settings m_proxy_settings;
		connection_queue& m_cc;
		tcp::resolver m_resolver;
		char m_tmp_buf[270];
		bool m_queue_packets;
		bool m_tunnel_packets;
		bool m_force_proxy;
		bool m_abort;

		udp::endpoint m_proxy_addr;
		udp::endpoint m_udp_proxy_addr;
		std::deque<queued_packet> m_queue;
		char timeout_stack[2000];
	};

	struct rate_limited_udp : public udp
	{
		rate_limited_udp(io_service& ios, connection_queue& cc);
		void set_rate_limit(int limit) { m_rate_limit = limit; }
		bool send(udp::endpoint const& ep, char const* p, int len
			, error& ec, int flags = 0);
	};
}



