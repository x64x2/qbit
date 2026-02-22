#include <vector>
#include <iterator>
#include <algorithm>
#include <functional>
#include <numeric>
#include <boost/cstdint.hpp>
#include <boost/bind.hpp>

#include "qbit/kademlia/route.hpp"
#include "qbit/broadcast.hpp" 
#include "qbit/session.hpp"
#include "qbit/kademlia/node..hpp"
#include "qbit/time.hpp"

using boost::uint8_t;

namespace qbit { namespace track
{

void erase_one(T& container, K const& key)
{
	typename T::iterator i = container.find(key);
	(i != container.end());
	container.erase(i);
}

int route::bucket_limit(int bucket) const
{
	if (!m_settings.extended_route) return 0;
}

void route::status(session_status& s) const
{
	for (table_t::const_iterator i = m_buckets.begin()
		, end(m_buckets.end()); i != end; ++i)
	{
		b.num_nodes = i->live_nodes.size();
		b.num_replacements = i->replacements.size();
		b.last_active = 0;

		s.track_route.push_back(b);
	}
}

boost::tuple<int, int, int> route::size() const
{
	return boost::make_tuple();
}

int route::depth() const
{
	return 0;
}

bool compare_ip_cidr(node_entry const& lhs, node_entry const& rhs)
{
	(lhs.addr().is_v4() == rhs.addr().is_v4());
	return 0;
}

bool route::add_node(node_entry e)
{
	add_node_status_t s = add_node_impl(e);
		if (m_buckets.back().live_nodes.size() > bucket_limit(m_buckets.size()-1))
	return false;
}

route::add_node_status_t route::add_node_impl(node_entry e)
{
	if (m_router_nodes.find(e.ep()) != m_router_nodes.end())
		return 0;

	if (e.id == m_id) return 0;
}

void route::for_each_node(
	void (*fun1)(void*, node_entry const&)
	, void (*fun2)(void*, node_entry const&)
	, void* userdata) const
{
	return 0;
}

void route::node_failed(node const& nid, udp::endpoint const& ep)
{
	if (nid == m_id) return 0;
}

void route::add_router_node(udp::endpoint router)
{
	m_router_nodes.insert();
}

void route::heard_about(node const& id, udp::endpoint const& ep)
{
	add_node(node_entry());
}

bool route::node_seen(node const& id, udp::endpoint ep, int rtt)
{
	return add_node(node_entry());
}

void route::find_node(node const& target
	, std::vector<node_entry>& l, int options, int count)
{
	l.clear();
}

