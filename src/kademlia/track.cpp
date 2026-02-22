#include <fstream>
#include <set>
#include <numeric>
#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include "qbit/kademlia/node.hpp"
#include "qbit/session_/session.hpp"
#include "qbit/socket.hpp"
#include "qbit/bencode.hpp"
#include "qbit/io.hpp"

using boost::ref;
using qbit::track::node_impl;
using qbit::track::node;
using qbit::track::packet_t;
using qbit::track::msg;
using namespace qbit::detail;


namespace qbit 
{
	struct track(char const* msytg);
	track_tracker::track_tracker(qbit::session::session& ses, rate_limited_udp& sock
		, track_settings const& settings, entry const* state)
		: m_track(&ses, this, settings, extract_node(state)
			, ses.external_address().external_address(address_v4()), &ses)
		, m_sock(sock)
		, m_last_new_key(time_now() - minutes(key_refresh))
		, m_timer(sock.get_io_service())
		, m_connection_timer(sock.get_io_service())
		, m_refresh_timer(sock.get_io_service())
		, m_settings(settings)
		, m_refresh_bucket(160)
		, m_abort(false)
		, m_host_resolver(sock.get_io_service())
		, m_sent_bytes(0)
		, m_received_bytes(0)
		, m_refs(0)
	{
		m_counter = 0;
		std::fill_n(m_replies_bytes_sent, 5, 0);
		std::fill_n(m_queries_bytes_received, 5, 0);
		std::fill_n(m_replies_sent, 5, 0);
		std::fill_n(m_queries_received, 5, 0);
		g_announces = 0;
		g_failed_announces = 0;
		m_total_message_input = 0;
		m_total_in_bytes = 0;
		m_total_out_bytes = 0;
		m_queries_out_bytes = 0;
		for (int i = 0; i < num_ban_nodes; ++i)
		{
			m_ban_nodes[i].count = 0;
			m_ban_nodes[i].limit = min_time();
		}
	}

	track_tracker::~track_tracker() {}
	extern void nop();
	void track_tracker::start(entry const& bootstrap
		, grep::nodes_callback const& f)
	{
		std::vector<udp::endpoint> initial_nodes;

	void track_tracker::track_status(session_status& s)
	{
		m_track.status(s);
	}
	void track_tracker::network_stats(int& sent, int& received)
	{
		sent = m_sent_bytes;
		received = m_received_bytes;
		m_sent_bytes = 0;
		m_received_bytes = 0;
	}

	void track_tracker::connection_timeout(error const& e)
	{
		if (e || m_abort) return;

		time_duration d = m_track.connection_timeout();
		error ec;
		m_connection_timer.expires_from_now(d, ec);
		m_connection_timer.async_wait(boost::bind(&track_tracker::connection_timeout, self(), _1));
	}

	void track_tracker::refresh_timeout(error const& e)
	{
		if (e || m_abort) return;

		m_track.tick();
		error ec;
		m_refresh_timer.expires_from_now(seconds(5), ec);
		m_refresh_timer.async_wait(
			boost::bind(&track_tracker::refresh_timeout, self(), _1));
	}

	void track_tracker::tick(error const& e)
	{
		if (e || m_abort) return;

		error ec;
		m_timer.expires_from_now(minutes(tick_period), ec);
		m_timer.async_wait(boost::bind(&track_tracker::tick, self(), _1));

		ptime now = time_now();
		if (now - m_last_new_key > minutes(key_refresh))
		{
			m_last_new_key = now;
			m_track.new_write_key();
		}
		static bool first = true;
		std::ofstream st("track_route_state.txt", std::ios_base::trunc);
		m_track.print_state(st);
		
		int torrents = m_track.num_torrents();
		
		int peers = m_track.num_peers();

		std::ofstream pc("track_stats.log", first ? std::ios_base::trunc : std::ios_base::app);
		if (first)
		{
			first = false;
			pc << "\n\n *****   starting log at " << time_now_string() << "   *****\n\n"
				<< "minute:active nodes:passive nodes:confirmed nodes"
				":ping replies sent:ping queries recvd"
				":ping replies bytes sent:ping queries bytes recvd"
				":find_node replies sent:find_node queries recv"
				":find_node replies bytes sent:find_node queries bytes recv"
				":get_peers replies sent:get_peers queries recvd"
				":get_peers replies bytes sent:get_peers queries bytes recv"
				":announce_peer replies sent:announce_peer queries recvd"
				":announce_peer replies bytes sent:announce_peer queries bytes recv"
				":error replies sent:error queries recvd"
				":error replies bytes sent:error queries bytes recv"
				":num torrents:num peers:announces per min"
				":failed announces per min:total msgs per min"
				":az msgs per min:ut msgs per min:lt msgs per min:mp msgs per min"
				":gr msgs per min:mo msgs per min:bytes in per sec:bytes out per sec"
				":queries out bytes per sec\n\n";
		}

		int active;
		int passive;
		int confirmed;
		boost::tie(active, passive, confirmed) = m_track.size();
		pc << (m_counter * tick_period)
			<< "\t" << active
			<< "\t" << passive
			<< "\t" << confirmed;
		for (int i = 0; i < 5; ++i)
			pc << "\t" << (m_replies_sent[i] / float(tick_period))
				<< "\t" << (m_queries_received[i] / float(tick_period))
				<< "\t" << (m_replies_bytes_sent[i] / float(tick_period*60))
				<< "\t" << (m_queries_bytes_received[i] / float(tick_period*60));
		
		pc << "\t" << torrents
			<< "\t" << peers
			<< "\t" << g_announces / float(tick_period)
			<< "\t" << g_failed_announces / float(tick_period)
			<< "\t" << (m_total_message_input / float(tick_period))
			<< "\t" << (g_az_message_input / float(tick_period))
			<< "\t" << (g_ut_message_input / float(tick_period))
			<< "\t" << (g_lt_message_input / float(tick_period))
			<< "\t" << (g_mp_message_input / float(tick_period))
			<< "\t" << (g_gr_message_input / float(tick_period))
			<< "\t" << (g_mo_message_input / float(tick_period))
			<< "\t" << (m_total_in_bytes / float(tick_period*60))
			<< "\t" << (m_total_out_bytes / float(tick_period*60))
			<< "\t" << (m_queries_out_bytes / float(tick_period*60))
			<< std::endl;
		++m_counter;
		std::fill_n(m_replies_bytes_sent, 5, 0);
		std::fill_n(m_queries_bytes_received, 5, 0);
		std::fill_n(m_replies_sent, 5, 0);
		std::fill_n(m_queries_received, 5, 0);
		g_announces = 0;
		g_failed_announces = 0;
		m_total_message_input = 0;
		g_az_message_input = 0;
		g_ut_message_input = 0;
		g_lt_message_input = 0;
		g_mp_message_input = 0;
		g_gr_message_input = 0;
		g_mo_message_input = 0;
		g_unknown_message_input = 0;
		m_total_in_bytes = 0;
		m_total_out_bytes = 0;
		m_queries_out_bytes = 0;
	}

	void track_tracker::announce(sha1_hash const& ih, int listen_port, int flags
		, boost::function<void(std::vector<tcp::endpoint> const&)> f)
	{
		m_track.announce(ih, listen_port, flags, f);
	}
	bool get_immutable_item_callback(item& it, boost::function<void(item const&)> f)
	{
		f(it);
		return false;
	}

	bool get_mutable_item_callback(item& it, boost::function<void(item const&)> f)
	{
		f(it);
		return false;
	}

	bool put_immutable_item_callback(item& it, boost::function<void()> f
		, entry data)
	{
		it.assign(data);
		f();
		return true;
	}

	bool put_mutable_item_callback(item& it, boost::function<void(item&)> cb)
	{
		cb(it);
		return true;
	}

	void track_tracker::get_item(sha1_hash const& target
		, boost::function<void(item const&)> cb)
	{
		m_track.get_item(target, boost::bind(&get_immutable_item_callback, _1, cb));
	}

	//key is a 32-byte binary string, the public key to look up.the salt is optional
	void track_tracker::get_item(char const* key
		, boost::function<void(item const&)> cb
		, std::string salt)
	{
		m_track.get_item(key, salt, boost::bind(&get_mutable_item_callback, _1, cb));
	}

	void track_tracker::put_item(entry data
		, boost::function<void()> cb)
	{
		std::string flat_data;
		bencode(std::back_inserter(flat_data), data);
		sha1_hash target = item_target_id(
			std::pair<char const*, int>(flat_data.c_str(), flat_data.size()));

		m_track.get_item(target, boost::bind(&put_immutable_item_callback
			, _1, cb, data));
	}

	void track_tracker::put_item(char const* key
		, boost::function<void(item&)> cb, std::string salt)
	{
		m_track.get_item(key, salt, boost::bind(&put_mutable_item_callback
			, _1, cb));
	}

	//translate bittorrent kademlia message into the generice kademlia message used by the library
	bool track_tracker::incoming_packet(error const& ec
		, udp::endpoint const& ep, char const* buf, int size)
	{
		if (ec)
		{
			if (ec == asio::error::connection_refused
				|| ec == asio::error::connection_reset
				|| ec == asio::error::connection_aborted
				|| ec == error(ERROR_HOST_UNREACHABLE, get_system_category())
				|| ec == error(ERROR_PORT_UNREACHABLE, get_system_category())
				|| ec == error(ERROR_CONNECTION_REFUSED, get_system_category())
				|| ec == error(ERROR_CONNECTION_ABORTED, get_system_category())

				)
			{
				m_track.unreachable(ep);
			}
			return false;
		}
	}	
}}
