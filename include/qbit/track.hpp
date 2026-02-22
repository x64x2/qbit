#ifndef TRACK__HPP
#define TRACK__HPP

#include <vector>
#include <string>
#include <utility>
#include <ctime>

#include <boost/cstdint.hpp>
#include <boost/tuple/tuple.hpp>

#include "qbit/socket.hpp"
#include "qbit/peer.hpp" 
#include "qbit/session.hpp" 
#include "qbit/time.hpp"
#include "qbit/udp.hpp" 
#include <boost/asio/ssl/context.hpp>


namespace qbit
{
	struct request_callback;
	class track;
	struct tracker_connection;
	namespace session { struct session; }

	struct tracker_request
	{
		tracker_request()
			: kind(announce_request)
			, downloaded(-1)
			, uploaded(-1)
			, left(-1)
			, corrupt(0)
			, redundant(0)
			, listen_port(0)
			, event(none)
			, key(0)
			, num_want(0)
			, send_stats(true)
			, apply_ip_filter(true)
			, ssl_ctx(0)

		{}

		enum
		{
			announce_request,
			scrape_request
		} kind;

		enum event_t
		{
			none,
			completed,
			started,
			stopped,
			paused
		};

		sha1_hash info_hash;
		peer_id pid;
		size_type downloaded;
		size_type uploaded;
		size_type left;
		size_type corrupt;
		size_type redundant;
		unsigned short listen_port;
		event_t event;
		std::string url;
		std::string trackerid;
		boost::uint32_t key;
		int num_want;
		address bind_ip;
		bool send_stats;
		bool apply_ip_filter;
		boost::asio::ssl::context* ssl_ctx;

	};

	struct request_callback
	{
		friend class track;
		request_callback(): m_manager(0) {}
		virtual ~request_callback() {}
		virtual void tracker_warning(tracker_request const& req
			, std::string const& msg) = 0;
		virtual void tracker_scrape_response(tracker_request const& /*req*/
			, int /*complete*/, int /*incomplete*/, int /*downloads*/
			, int /*downloaders*/) {}
		virtual void tracker_response(
			tracker_request const& req
			, address const& tracker_ip
			, std::list<address> const& ip_list
			, std::vector<peer_entry>& peers
			, int interval
			, int min_interval
			, int complete
			, int incomplete
			, int downloaded
			, address const& external_ip
			, std::string const& trackerid) = 0;
		virtual void tracker_request_error(
			tracker_request const& req
			, int response_code
			, error const& ec
			, const std::string& msg
			, int retry_interval) = 0;
			
			virtual void log(const char* fmt, ...) const = 0;

		void set_timeout(int completion_timeout, int read_timeout);
		void restart_read_timeout();
		void cancel();
		bool cancelled() const { return m_abort; }

		virtual void on_timeout(error const& ec) = 0;
		virtual ~timeout_handler() {}

		typedef mutex mutex_t;
		mutable mutex_t m_mutex;
		bool m_abort;
	};

		void fail(error const& ec, int code = -1, char const* msg = ""
			, int interval = 0, int min_interval = 0);
		virtual void start() = 0;
		virtual void close();
		address const& bind_interface() const { return m_req.bind_ip; }
		void sent_bytes(int bytes);
		void received_bytes(int bytes);
		virtual bool on_receive(error const& ec, udp::endpoint const& ep
			, char const* buf, int size) { return false; }
		virtual bool on_receive_hostname(error const& ec, char const* hostname
			, char const* buf, int size) { return false; }

		void fail_impl(error const& ec, int code = -1, std::string msg = std::string()
			, int interval = 0, int min_interval = 0);

		boost::weak_ptr<request_callback> m_requester;
	};

	class track: public udp_observer, boost::noncopyable
	{
	public:

		track(session::session& ses, proxy_settings const& ps)
			: m_ses(ses)
			, m_proxy(ps)
			, m_abort(false) {}
		~track();

		void sent_bytes(int bytes);
		void received_bytes(int bytes);

		virtual bool incoming_packet(error const& e, udp::endpoint const& ep
			, char const* buf, int size);

		virtual bool incoming_packet(error const& e, char const* hostname
			, char const* buf, int size);
		
	private:

		typedef mutex mutex_t;
		mutable mutex_t m_mutex;

		typedef std::list<boost::intrusive_ptr<tracker_connection> >
			tracker_connections_t;
		tracker_connections_t m_connections;
		session::session& m_ses;
		proxy_settings const& m_proxy;
		bool m_abort;
};