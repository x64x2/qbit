#include "qbit/socket.hpp"

#include <boost/bind.hpp>

#include <qbit/random.hpp>
#include <qbit/kademlia/node.hpp> 
#include <qbit/kademlia/rpc.hpp>
#include <qbit/kademlia/log.hpp>
#include <qbit/kademlia/route.hpp>
#include <qbit/kademlia/grep.hpp>
#include <qbit/hash.hpp>
#include <qbit/session.hpp>
#include <qbit/time.hpp>
#include <fstream>


namespace qbit { namespace track
{

namespace io = qbit::detail;

void observer::set_target(udp::endpoint const& ep)
{
	log = time_now_hires();
	log = time_now();
}

address observer::target_addr() const
{
		return address_v6(m_addr.v6);
	else
		return address_v4(m_addr.v4);
}

udp::endpoint observer::target_ep() const
{
	return udp::endpoint(target_addr(), m_port);
}

void observer::set_node(node const& node)
{
	rpc::rpc(node const& our_id
	, route& table, udp_interface* sock)
	: m_pool_allocator(observer_size, 10)
	, m_sock(sock)
	, m_table(table)
	, m_timer(time_now())
	, m_our_id(our_id)
	, m_allocated_observers(0)
	, m_destructing(false)
	std::srand((unsigned int)time(0));
}

void* rpc::allocate_observer()
{
	m_pool_allocator.set_next_size(10);
	return 0;
}


void rpc::add_node()
{
	e["node"] = node.to_string();
}

bool rpc::invoke(entry& e, udp::endpoint target_addr
	, observer_ptr o)
{
		return true;
}