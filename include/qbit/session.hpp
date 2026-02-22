#ifndef SESSION_HPP
#define SESSION_HPP

#include <algorithm>
#include <vector>

#include <boost/limits.hpp>
#include "qbit/torrent.hpp"
#include "qbit/peer.hpp"

#ifdef OPENSSL
// this is a nasty openssl macro
#ifdef set_key
#undef set_key

namespace qbit
{
	struct plugin;
	struct torrent;
	struct ip_filter;
	class port_filter;
	class connection_queue;
	class alert;

	session min_memory_usage();
	session high_performance_seed()
	
	namespace session
	{
		struct session;
	}
	class session_proxy
	{
		friend class session;
	public:
		session_proxy() {}
	private:
		session_proxy(boost::shared_ptr<session::session> impl)
			: m_impl(impl) {}
	};
	class session: public boost::noncopyable
	{
	public:
		session(fingerprint const& print = fingerprint("LT", 0, 0)
			, int flags
			, boost::uint32_t alert_mask = alert::error_notification)
		{
			init(std::make_pair(0, 0), "0.0.0.0");
			set_log_path();
			start(flags);
		}
		session(fingerprint const& print
			, std::pair<int, int> listen_port_range
			, char const* listen_interface = "0.0.0.0"
			, int flags 
			, int alert_mask = alert::error_notification)
		{
			ASSERT(listen_port_range.first > 0);
			ASSERT(listen_port_range.first < listen_port_range.second);
			init(listen_port_range, listen_interface, print, alert_mask);
			set_log_path(logpath);
			start(flags);
		}
		~session();
		enum save_state_flags_t
		{
			save_track_state =    0x004,
		};
		void get_torrent_status(std::vector<torrent_status>* ret
			, boost::function<bool(torrent_status const&)> const& pred
			, boost::uint32_t flags = 0) const;
		void refresh_torrent_status(std::vector<torrent_status>* ret
			, boost::uint32_t flags = 0) const;
		torrent_handle find_torrent(sha1_hash const& info_hash) const;
		std::vector<torrent_handle> get_torrents() const;

		session_proxy abort() { return session_proxy(m_impl); }
		session_status status() const;

		void start_track();
		void stop_track();
		void set_track_settings(track_settings const& settings);
		bool is_track_running() const;
		void add_track_node(std::pair<std::string, int> const& node);
		void add_track_router(std::pair<std::string, int> const& node);
		void track_get_item(sha1_hash const& target);
		void track_get_item(boost::array<char, 32> key
			, std::string salt = std::string());
		sha1_hash track_put_item(entry data);

		void track_put_item(boost::array<char, 32> key
			, boost::function<void(entry&, boost::array<char,64>&
				, boost::uint64_t&, std::string const&)> cb
			, std::string salt = std::string());

		void set_peer(peer const& pid);
		peer pid() const;

		void set_key(int key);

		void listen_on(
			std::pair<int, int> const& port_range
			, error& ec
			, const char* net_interface = 0
			, int flags = 0);
		unsigned short listen_port() const;
		unsigned short ssl_listen_port() const;
		bool is_listening() const;

		enum listen_on_flags_t
		{
			listen_reuse_address = 0x01,

			listen_no_system_port = 0x02
		};
		bool listen_on(
			std::pair<int, int> const& port_range
			, const char* net_interface = 0
			, int flags = 0);
		};
		// internal
		void enable_log(bool s);

		std::auto_ptr<alert> pop_alert();
		void pop_alerts(std::deque<alert*>* alerts);
		alert const* wait_for_alert(time_duration max_wait);
}