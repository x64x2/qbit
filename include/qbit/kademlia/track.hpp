#ifndef TRACKER
#define TRACKER

#include <fstream>
#include <set>
#include <numeric>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/detail/atomic_count.hpp>

#include "qbit/kademlia/node.hpp"
#include "qbit/session_status.hpp"
#include "qbit/socket.hpp"
#include "qbit/thread.hpp"
#include "qbit/timer.hpp"

namespace qbit
{
	namespace session { struct session; }
}

namespace qbit { namespace track
{
	struct track;

	void intrusive_ptr_add_ref(track const*);
	void intrusive_ptr_release(track const*);	

	struct tracker:udp_interface
	{
		friend void intrusive_ptr_add_ref(track const*);
		friend void intrusive_ptr_release(track const*);

		track(qbit::session::session& ses, rate_limited_socket& sock
			, track_settings const& settings, entry const* state = 0);
		virtual ~track();

		void start(entry const& bootstrap
			, grep::nodes_callback const& f);
		void stop();

		void add_node(udp::endpoint node);
		void add_node(std::pair<std::string, int> const& node);
		void add_router_node(udp::endpoint const& node);

		entry state() const;

		enum flags_t { flag_seed = 1, flag_implied_port = 2 };
		void announce(sha1_hash const& ih, int listen_port, int flags, boost::function<void(std::vector<tcp::endpoint> const&)> f);

		void get_item(sha1_hash const& target, boost::function<void(item const&)> cb);

		//key is a 32-byte binary string, the public key to look up,the salt is optional
		void get_item(char const* key, boost::function<void(item const&)> cb, std::string salt = std::string());

		void put_item(entry data, boost::function<void()> cb);

		void put_item(char const* key, boost::function<void(item&)> cb, std::string salt = std::string());

		void status(session_status& s);
		void network_stats(int& sent, int& received);

		//translate bittorrent kademlia message into the generic kademlia message
		virtual bool incoming_packet(error const& ec
			, udp::endpoint const&, char const* buf, int size);

	private:
	
		boost::intrusive_ptr<track> self()
		{ return boost::intrusive_ptr<track>(this); }

		void on_name_lookup(error const& e
			, udp::resolver::iterator host);
		void on_router_name_lookup(error const& e
			, udp::resolver::iterator host);
		void connection_timeout(error const& e);
		void refresh_timeout(error const& e);
		void tick(error const& e);

		//implements udp_interface
		virtual bool send_packet(qbit::entry& e, udp::endpoint const& addr
			, int send_flags);
		
		//used to ignore abusive track nodes
		struct node_ban_entry
		{
			enum { num_ban_nodes = 20 };
		};
}}



