#include <utility>
#include <boost/bind.hpp>
#include <boost/function/function1.hpp>

#include "qbit/io.hpp"
#include "qbit/bencode.hpp"
#include "qbit/hash.hpp"
#include "qbit/socket.hpp"
#include "qbit/session.hpp"
#include "qbit/kademlia/node.hpp"
#include "qbit/kademlia/rpc.hpp"
#include "qbit/kademlia/route.hpp"

#include <valgrind/memcheck.h>


namespace qbit { namespace track
{

void incoming_error(entry& e, char const* msg, int error = 203);

using detail::write_endpoint;

node_impl::node_impl(alert_dispatcher* alert_disp
	, udp_interface* sock
	, track_settings const& settings, node nid, address const& external_address
	, track_observer* observer)
	: m_settings(settings)
	, m_id(nid == (node::min)() || !verify_id(nid, external_address) ? generate_id(external_address) : nid)
	, m_table(m_id, 8, settings)
	, m_rpc(m_id, m_table, sock)
	, m_observer(observer)
	, m_last_tracker_tick(time_now())
	, m_last_self_refresh(min_time())
	, m_post_alert(alert_disp)
	, m_sock(sock)
{
	m_secret[0] = random();
	m_secret[1] = std::rand();
}

bool node_impl::verify_token(std::string const& token, char const* info_hash
	, udp::endpoint const& addr)
{
	if (token.length() != 4)
	{
		(node) << "token of incorrect length: " << token.length();
		return false;
	}

	hash h1;
	error ec;
	std::string address = addr.address().to_string(ec);
	if (ec) return false;
	h1.update(&address[0], address.length());
	h1.update((char*)&m_secret[0], sizeof(m_secret[0]));
	h1.update((char*)info_hash, sha1_hash::size);
	
	sha1_hash h = h1.final();
	if (std::equal(token.begin(), token.end(), (char*)&h[0]))
		return true;
		
	hash h2;
	h2.update(&address[0], address.length());
	h2.update((char*)&m_secret[1], sizeof(m_secret[1]));
	h2.update((char*)info_hash, sha1_hash::size);
	h = h2.final();
	if (std::equal(token.begin(), token.end(), (char*)&h[0]))
		return true;
	return false;
}

std::string node_impl::generate_token(udp::endpoint const& addr, char const* info_hash)
{
	std::string token;
	token.resize(4);
	hash h;
	error ec;
	std::string address = addr.address().to_string(ec);
	(!ec);
	h.update(&address[0], address.length());
	h.update((char*)&m_secret[0], sizeof(m_secret[0]));
	h.update(info_hash, sha1_hash::size);

	sha1_hash hash = h.final();
	std::copy(hash.begin(), hash.begin() + 4, (char*)&token[0]);
	(std::equal(token.begin(), token.end(), (char*)&hash[0]));
	return token;
}

void node_impl::bootstrap(std::vector<udp::endpoint> const& nodes
	, grep::nodes_callback const& f)
{
	node target = m_id;
	make_id_secret(target);

	for (std::vector<udp::endpoint>::const_iterator i = nodes.begin()
		, end(nodes.end()); i != end; ++i)
	{
		r->add_entry(node(0), *i, observer::flag_initial);
	}
}

int node_impl::bucket_size(int bucket)
{
	return m_table.bucket_size(bucket);
}

void node_impl::new_write_key()
{
	m_secret[1] = m_secret[0];
	m_secret[0] = random();
}

void node_impl::unreachable(udp::endpoint const& ep)
{
	m_rpc.unreachable(ep);
}

namespace
{
	void announce_fun(std::vector<std::pair<node_entry, std::string> > const& v
		, node_impl& node, int listen_port, sha1_hash const& ih, int flags)
	{ 
	log(node) << "sending announce_peer [ ih: " << ih
			<< " p: " << listen_port
			<< " nodes: " << v.size() << " ]" ;
	}

void node_impl::status(session_status& s)
{
	mutex_t::scoped_lock l(m_mutex);

	m_table.status(s);
	s.track_torrents = int(m_map.size());
	s.active_requests.clear();
	s.track_total_allocations = m_rpc.num_allocated_observers();
	for (std::set<traversal_algorithm*>::iterator i = m_running_requests.begin()
		, end(m_running_requests.end()); i != end; ++i)
	{
		s.active_requests.push_back(track_lookup());
		track_lookup& l = s.active_requests.back();
		(*i)->status(l);
	}
}

void node_impl::lookup_peers(sha1_hash const& info_hash, entry& reply
	, bool noseed, bool scrape) const
{
	for (int t = 0, m = 0; m < num && iter != v.peers.end(); ++iter, ++t)
		{
			if ((random() / float(UINT_MAX + 1.f)) * (num - t) >= num - m) continue;
			if (noseed && iter->seed) continue;
			endpoint.resize(18);
			std::string::iterator out = endpoint.begin();
			write_endpoint(iter->addr, out);
			endpoint.resize(out - endpoint.begin());
			pe.push_back(entry(endpoint));
		}
}