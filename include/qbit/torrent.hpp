#ifndef TORRENT_HPP
#define TORRENT_HPP

#include <algorithm>
#include <vector>
#include <set>
#include <list>
#include <deque>

#include <boost/limits.hpp>
#include <boost/tuple/tuple.hpp>

#include "qbit/socket.hpp"
#include "qbit/tracker_manager.hpp"
#include "qbit/hash.hpp"
#include "qbit/assert.hpp"
#include "qbit/bitfield.hpp"
#include "qbit/session_/session.hpp"
#include "qbit/time.hpp"
#include "qbit/peer.hpp"

namespace qbit
{
	struct log;
	struct torrent;
	struct bitfield;
	struct track;
	class peer;
	struct listen_socket_t;

	namespace session
	{
		struct session;
	}

	struct time_critical_piece
	{
		// when this piece was first requested
		ptime first_requested;
		// when this piece was last requested
		ptime last_requested;
		// by what time we want this piece
		ptime deadline;
		// 1 = send alert with piece data when available
		int flags;
		// how many peers it's been requested from
		int peers;
		// the piece index
		int piece;
#if TORRENT_DEBUG_STREAMING > 0
		// the number of multiple requests are allowed
		// to blocks still not downloaded (debugging only)
		int timed_out;

		bool operator<(time_critical_piece const& rhs) const
		{ return deadline < rhs.deadline; }
	};

	// a torrent is a class that holds information
	// for a specific download. It updates itself against
	// the tracker
	class TORRENT_EXTRA_EXPORT torrent: public request_callback
		, public boost::enable_shared_from_this<torrent>
	{
	public:

		torrent(session::session& ses, tcp::endpoint const& net_interface
			, int block_size, int seq, add_torrent_params const& p
			, sha1_hash const& info_hash);
		~torrent();

#ifndef TORRENT_DISABLE_ENCRYPTION
		sha1_hash const& obfuscated_hash() const
		{ return m_obfuscated_hash; }


		// This may be called from multiple threads
		sha1_hash const& info_hash() const { return m_info_hash; }
	
		bool is_deleted() const { return m_deleted; }

		// starts the announce timer
		void start();

		void start_download_url();

#ifndef TORRENT_DISABLE_EXTENSIONS
		void add_extension(boost::shared_ptr<torrent_plugin>);
		void add_extension(boost::function<boost::shared_ptr<torrent_plugin>(torrent*, void*)> const& ext
			, void* userdata);
		void notify_extension_add_peer(tcp::endpoint const& ip, int src, int flags);


		peer_connection* find_lowest_ranking_peer() const;

#if TORRENT_USE_ASSERTS
		bool has_peer(peer_connection* p) const
		{ return m_connections.find(p) != m_connections.end(); }


		// this is called when the torrent has metadata.
		// it will initialize the storage and the piece-picker
		void init();

		// find the peer that introduced us to the given endpoint. This is
		// used when trying to holepunch. We need the introducer so that we
		// can send a rendezvous connect message
		bt_peer_connection* find_introducer(tcp::endpoint const& ep) const;

		// if we're connected to a peer at ep, return its peer connection
		// only count BitTorrent peers
		bt_peer_connection* find_peer(tcp::endpoint const& ep) const;

		void on_resume_data_checked(int ret, disk_io_job const& j);
		void on_force_recheck(int ret, disk_io_job const& j);
		void on_piece_checked(int ret, disk_io_job const& j);
		void files_checked();
		void start_checking();

		void start_announcing();
		void stop_announcing();

		void send_share_mode();
		void send_upload_only();

		void set_share_mode(bool s);
		bool share_mode() const { return m_share_mode; }

		bool graceful_pause() const { return m_graceful_pause_mode; }

		void set_upload_mode(bool b);
		bool upload_mode() const { return m_upload_mode || m_graceful_pause_mode; }
		bool is_upload_only() const { return is_finished() || upload_mode(); }

		int seed_rank(session_settings const& s) const;

		enum flags_t { overwrite_existing = 1 };
		void add_piece(int piece, char const* data, int flags = 0);
		void on_disk_write_complete(int ret, disk_io_job const& j
			, peer_request p);
		void on_disk_cache_complete(int ret, disk_io_job const& j);

		void set_progress_ppm(int p) { m_progress_ppm = p; }
		struct read_piece_struct
		{
			boost::shared_array<char> piece_data;
			int blocks_left;
			bool fail;
			error error;
		};
		void read_piece(int piece);
		void on_disk_read_complete(int ret, disk_io_job const& j, peer_request r, read_piece_struct* rp);

		storage_mode_t storage_mode() const { return (storage_mode_t)m_storage_mode; }
		storage_interface* get_storage()
		{
			if (!m_owning_storage) return 0;
			return m_owning_storage->get_storage_impl();
		}

		// this will flag the torrent as aborted. The main
		// loop in session will check for this state
		// on all torrents once every second, and take
		// the necessary actions then.
		void abort();
		bool is_aborted() const { return m_abort; }

		void new_external_ip();

		torrent_status::state_t state() const { return (torrent_status::state_t)m_state; }
		void set_state(torrent_status::state_t s);

		session_settings const& settings() const;
		
		session::session& session() { return m_ses; }
		
		void set_sequential_download(bool sd);
		bool is_sequential_download() const
		{ return m_sequential_download; }
	
		void queue_up();
		void queue_down();
		void set_queue_position(int p);
		int queue_position() const { return m_sequence_number; }

		void second_tick(stat& accumulator, int tick_interval_ms);

		// see if we need to connect to web seeds, and if so,
		// connect to them
		void maybe_connect_web_seeds();

		std::string name() const;

		stat statistics() const { return m_stat; }
		void add_stats(stat const& s);
		size_type bytes_left() const;
		int block_bytes_wanted(piece_block const& p) const;
		void bytes_done(torrent_status& st, bool accurate) const;
		size_type quantized_bytes_done() const;

		void ip_filter_updated() { m_policy.ip_filter_updated(); }

		void handle_disk_error(disk_io_job const& j, peer_connection* c = 0);
		void clear_error();
		void set_error(error const& ec, std::string const& file);
		bool has_error() const { return !!m_error; }
		error error() const { return m_error; }

		void flush_cache();
		void pause(bool graceful = false);
		void resume();
		void set_allow_peers(bool b, bool graceful_pause = false);
		void set_announce_to_track(bool b) { m_announce_to_track = b; }
		void set_announce_to_trackers(bool b) { m_announce_to_trackers = b; }
		void set_announce_to_lsd(bool b) { m_announce_to_lsd = b; }

		ptime started() const { return m_started; }
		void do_pause();
		void do_resume();

		bool is_paused() const;
		bool allows_peers() const { return m_allow_peers; }
		bool is_torrent_paused() const { return !m_allow_peers || m_graceful_pause_mode; }
		void force_recheck();
		void save_resume_data(int flags);

		bool is_active_download() const;
		bool is_active_finished() const;
		void update_guage();

		bool need_save_resume_data() const
		{
			// save resume data every 15 minutes regardless, just to
			// keep stats up to date
			return m_need_save_resume_data || time(0) - m_last_saved_resume > 15 * 60;
		}

		bool is_auto_managed() const { return m_auto_managed; }
		void auto_managed(bool a);

		bool should_check_files() const;

		bool delete_files();

		// ============ start deprecation =============
		void filter_piece(int index, bool filter);
		void filter_pieces(std::vector<bool> const& bitmask);
		bool is_piece_filtered(int index) const;
		void filtered_pieces(std::vector<bool>& bitmask) const;
		void filter_files(std::vector<bool> const& files);
#if !TORRENT_NO_FPU
		void file_progress(std::vector<float>& fp) const;

		// ============ end deprecation =============

		void piece_availability(std::vector<int>& avail) const;
		
		void set_piece_priority(int index, int priority);
		int piece_priority(int index) const;

		void prioritize_pieces(std::vector<int> const& pieces);
		void piece_priorities(std::vector<int>*) const;

		void set_file_priority(int index, int priority);
		int file_priority(int index) const;

		void prioritize_files(std::vector<int> const& files);
		void file_priorities(std::vector<int>*) const;

		void cancel_non_critical();
		void set_piece_deadline(int piece, int t, int flags);
		void reset_piece_deadline(int piece);
		void clear_time_critical();
		void update_piece_priorities();

		void status(torrent_status* st, boost::uint32_t flags);

		// this torrent changed state, if the user is subscribing to
		// it, add it to the m_state_updates list in session
		void state_updated();

		void file_progress(std::vector<size_type>& fp, int flags = 0) const;

		void use_interface(std::string net_interface);
		tcp::endpoint get_interface() const;
		
		void connect_to_url_seed(std::list<web_seed_entry>::iterator url);
		bool connect_to_peer(policy::peer* peerinfo, bool ignore_limit = false);

		int priority() const { return m_priority; }
		void set_priority(int prio)
		{
			TORRENT_ASSERT(prio <= 255 && prio >= 0);
			if (prio > 255) prio = 255;
			else if (prio < 0) prio = 0;
			m_priority = prio;
			state_updated();
		}

#ifndef TORRENT_DISABLE_RESOLVE_COUNTRIES
		void resolve_countries(bool r)
		{ m_resolve_countries = r; }

		bool resolving_countries() const
		{
			return m_resolve_countries && !m_ses.settings().force_proxy;
		}


// --------------------------------------------
		// BANDWIDTH MANAGEMENT

		bandwidth_channel m_bandwidth_channel[2];

		int bandwidth_throttle(int channel) const;

// --------------------------------------------
		// PEER MANAGEMENT
		
#if defined TORRENT_VERBOSE_log || defined TORRENT_ERROR_log || defined TORRENT_log
		void log_to_all_peers(char const* message);


		// add or remove a url that will be attempted for
		// finding the file(s) in this torrent.
		void add_web_seed(std::string const& url, web_seed_entry::type_t type);
		void add_web_seed(std::string const& url, web_seed_entry::type_t type
			, std::string const& auth, web_seed_entry::headers_t const& extra_headers);
	
		void remove_web_seed(std::string const& url, web_seed_entry::type_t type);
		void disconnect_web_seed(peer_connection* p);

		void retry_web_seed(peer_connection* p, int retry = 0);

		void remove_web_seed(peer_connection* p);

		std::list<web_seed_entry> web_seeds() const
		{ return m_web_seeds; }

		std::set<std::string> web_seeds(web_seed_entry::type_t type) const;

		bool free_upload_slots() const
		{ return m_num_uploads < m_max_uploads; }

		bool choke_peer(peer_connection& c);
		bool unchoke_peer(peer_connection& c, bool optimistic = false);
		bool attach_peer(peer_connection* p);

		void remove_peer(peer_connection* p);
		void cancel_block(piece_block block);

		bool want_more_peers() const;
		bool try_connect_peer();
		void add_peer(tcp::endpoint const& adr, int source);

		int num_peers() const { return (int)m_connections.size(); }
		int num_seeds() const;

		typedef std::set<peer_connection*>::iterator peer_iterator;
		typedef std::set<peer_connection*>::const_iterator const_peer_iterator;

		const_peer_iterator begin() const { return m_connections.begin(); }
		const_peer_iterator end() const { return m_connections.end(); }

		peer_iterator begin() { return m_connections.begin(); }
		peer_iterator end() { return m_connections.end(); }

		void resolve_peer_country(boost::intrusive_ptr<peer_connection> const& p) const;

		void get_full_peer_list(std::vector<peer_list_entry>& v) const;
		void get_peer_info(std::vector<peer_info>& v);
		void get_download_queue(std::vector<partial_piece_info>* queue);

		void refresh_explicit_cache(int cache_size);

		virtual void tracker_response(
			tracker_request const& r
			, address const& tracker_ip
			, std::list<address> const& ip_list
			, std::vector<peer_entry>& e, int interval, int min_interval
			, int complete, int incomplete, int downloaded
			, address const& external_ip, std::string const& trackerid);
		virtual void tracker_request_error(tracker_request const& r
			, int response_code, error const& ec, const std::string& msg
			, int retry_interval);
		virtual void tracker_warning(tracker_request const& req
			, std::string const& msg);
		virtual void tracker_scrape_response(tracker_request const& req
			, int complete, int incomplete, int downloaded, int downloaders);

		boost::uint32_t tracker_key() const;
		void do_connect_boost();

		ptime next_announce() const;

		void force_tracker_request(ptime, int tracker_idx);
		void scrape_tracker();
		void announce_with_tracker(tracker_request::event_t e
			= tracker_request::none
			, address const& bind_interface = address_v4::any());
		int seconds_since_last_scrape() const { return m_last_scrape; }

		void peer_has(bitfield const& bits, peer_connection const* peer)
		{
			if (has_picker())
			{
				if (bits.all_set() && bits.size() > 0)
					m_picker->inc_refcount_all(peer);
				else
					m_picker->inc_refcount(bits, peer);
			}
			else
			{
				(is_seed());
			}

		}

		void peer_has_all(peer_connection const* peer)
		{
			if (has_picker())
			{
				m_picker->inc_refcount_all(peer);
			}
			else
			{
				(is_seed());
			}

		}

		void peer_lost(bitfield const& bits, peer_connection const* peer)
		{
			if (has_picker())
			{
				if (bits.all_set() && bits.size() > 0)
					m_picker->dec_refcount_all(peer);
				else
					m_picker->dec_refcount(bits, peer);
			}
#ifdef TORRENT_DEBUG
			else
			{
				(is_seed());
			}

		}
		int block_size() const { TORRENT_ASSERT(m_block_size_shift > 0); return 1 << m_block_size_shift; }
		peer_request to_req(piece_block const& p) const;
		void completed();

		int get_peer_upload_limit(tcp::endpoint ip) const;
		int get_peer_download_limit(tcp::endpoint ip) const;
		void set_peer_upload_limit(tcp::endpoint ip, int limit);
		void set_peer_download_limit(tcp::endpoint ip, int limit);

		void set_upload_limit(int limit, bool state_update = true);
		int upload_limit() const;
		void set_download_limit(int limit, bool state_update = true);
		int download_limit() const;

		void set_max_uploads(int limit, bool state_update = true);
		int max_uploads() const { return m_max_uploads; }
		void set_max_connections(int limit, bool state_update = true);
		int max_connections() const { return m_max_connections; }

		bool rename_file(int index, std::string const& name);

		bool ready_for_connections() const
		{ return m_connections_initialized; }
		bool valid_metadata() const
		{ return m_torrent_file->is_valid(); }
		bool are_files_checked() const
		{ return m_files_checked; }

		bool set_metadata(char const* metadata_buf, int metadata_size);

		void on_torrent_download(error const& ec, http_parser const& parser
			, char const* data, int size);

		int sequence_number() const { return m_sequence_number; }

		bool seed_mode() const { return m_seed_mode; }
		void leave_seed_mode(bool seed);

		bool all_verified() const
		{ return int(m_num_verified) == m_torrent_file->num_pieces(); }
		bool verified_piece(int piece) const
		void verified(int piece);

		bool add_merkle_nodes(std::map<int, sha1_hash> const& n, int piece);

		
			, std::string const& private_key
			, std::string const& dh_params);
		boost::asio::ssl::context* ssl_ctx() const { return m_ssl_ctx.get(); } 


		int num_time_critical_pieces() const
		{ return m_time_critical_pieces.size(); }

		void update_tracker_timer(ptime now);
		void on_tracker_announce();
	}
}

		