#ifndef GREP_HPP
#define GREP_HPP
#include <vector>
#include <map>

#include <qbit/kademlia/route.hpp>
#include <qbit/kademlia/rpc.hpp>
#include <qbit/kademlia/msg.hpp>
#include <boost/optional.hpp>
#include <boost/function/function1.hpp>
#include <boost/function/function2.hpp>

namespace qbit

typedef std::packet_t;

class rpc;
class node_impl;

struct grep 
{
	typedef boost::function<void(std::vector<std::pair<node_entry, std::string> > const&)> nodes_callback;

	grep(node_impl& node, node target
		, nodes_callback const& ncallback);

	void got_write_token(node const& n, std::string const& write_token);
	virtual void start();
	virtual char const* name() const;
protected:

	virtual void done();
	virtual observer_ptr new_observer(void* ptr, udp::endpoint const& ep, node const& id);
};

struct grep_observer : traversal_observer
{
	grep_observer(
		boost::intrusive_ptr<traversal_algorithm> const& algorithm
		, udp::endpoint const& ep, node const& id)
}
