#ifndef RPC_HPP
#define RPC_HPP

#include <vector>
#include <deque>
#include <map>
#include <boost/cstdint.hpp>
#include <boost/pool/pool.hpp>
#include <boost/function/function3.hpp>

#include <qbit/socket.hpp>
#include <qbit/kademlia/node.hpp>
#include <qbit/kademlia/log.hpp>
#include "qbit/ptime.hpp"

namespace qbit { namespace session { struct session; } }
namespace qbit { namespace track
{

class route;

class rpc
{
public:

	rpc(node const& node
		, route& table, udp_interface* sock);
	~rpc();
	void unreachable(udp::endpoint const& ep);
	bool incoming(node* node, qbit::track);
	bool invoke(entry& e, udp::endpoint target
		, observer_ptr o);

	void add_our_id(entry& e);
	size_t allocation_size() const;
	void check_invariant() const;

	void* allocate_observer();
	void free_observer(void* ptr);

	int num_allocated_observers() const { return m_allocated_observers; }
private:
	boost::uint32_t calc_connection_id(udp::endpoint addr);
};





