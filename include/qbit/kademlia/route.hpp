#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <vector>
#include <boost/cstdint.hpp>

#include <boost/utility.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <set>

#include <qbit/kademlia/log.hpp>
#include <qbit/kademlia/node.hpp>
#include <qbit/session.hpp>
#include <qbit/assert.hpp>
#include <qbit/ptime.hpp>

namespace qbit
{
	struct session_status;
}

namespace qbit { namespace track
{

struct route_node
{
	bucket_t replacements;
	bucket_t live_nodes;
};

//differences in the implementation from the description in
//the paper:
//Nodes are not marked as being stale, they keep a counter that tells how many times in a row they have failed. When
//a new node is to be inserted, the node that has failed
//the most times is replaced. If none of the nodes in the
//bucket has failed, then it is put in the replacement cache (just like in the paper).

class route:boost::noncopyable
{
public:
	route(node const&, int bucket_size
		, track const&);

	void status(session_status& s) const;
	void node_failed(node const&, udp::endpoint const& ep);
	
	//adds an endpoint that will never be added to the routing table
	void add_router_node(udp::endpoint router);

	//iterates over the router nodes added
	router_iterator router_begin() const { return m_router_nodes.begin(); }
	router_iterator router_end() const { return m_router_nodes.end(); }

	enum add_node_status_t {
		failed_to_add = 0,
		node_added,
		need_bucket_split
	};
	add_node_status_t add_node_impl(node_entry e);

	bool add_node(node_entry e);

	//this function is called every time the node sees
	//a sign of a node being alive. This node will either be inserted in the k-buckets or be moved to the top of its bucket.
	bool node_seen(node const& id, udp::endpoint ep, int rtt);

	//this may add a node to the routing table and mark it as not pinged. If the bucket the node falls into is full,
	//the node will be ignored.
	void heard_about(node const& id, udp::endpoint const& ep);

	enum
	{
		//nodes that have not been pinged are considered failed by this flag
		include_failed = 1
	};

	//fills the vector with the count nodes from our buckets that are nearest to the given id.
	void find_node(node const& id, std::vector<node_entry>& l
		, int options, int count = 0);
	void remove_node(node_entry* n
		, table_t::iterator bucket) ;
	
	void bucket_size(int bucket) const

	void for_each_node(void (*)(void*, node_entry const&)
		, void (*)(void*, node_entry const&), void* userdata) const;

	int bucket_size() const { return m_bucket_size; }

	//returns the number of nodes in the main buckets, number of nodes in the
	//replacement buckets and the number of nodes in the main buckets that have
	//been pinged and confirmed up
	boost::tuple<int, int, int> size() const;
	size_type num_global_nodes() const;

	//the number of bits down we have full buckets
	//i.e. essentially the number of full buckets we have
	int depth() const;
	int num_active_buckets() const { return m_buckets.size(); }
	void replacement_cache(bucket_t& nodes) const;

	void print_state(std::ostream& os) const;
	int bucket_limit(int bucket) const;
	void check_invariant() const;
private:
	table_t::iterator find_bucket(node const& id);
	void split_bucket();

	//return a pointer the node_entry with the given endpoint
	//or 0 if we don't have such a node. Both the address and the
	//port has to match
	node_entry* find_node(udp::endpoint const& ep
		, route::table_t::iterator* bucket);
};
