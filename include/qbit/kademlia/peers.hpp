#ifndef QBIT_PEERS_HPP
#define QBIT_PEERS_HPP

#include <qbit/kademlia/grep.hpp>

namespace qbit { namespace track
{

struct get_peers:grep
{
	void got_peers(std::vector<tcp::endpoint> const& peers);
	get_peers(node_impl& node, node target
		, data_callback const& dcallback
		, nodes_callback const& ncallback
		, bool noseeds);

	virtual char const* name() const;

protected:
	virtual bool invoke(observer_ptr o);
	virtual observer_ptr new_observer(void* ptr, udp::endpoint const& ep, node const& id);
};

struct obfuscated_get_peers:get_peers
{
	obfuscated_get_peers(node_impl& node, node target
		, data_callback const& dcallback
		, nodes_callback const& ncallback
		, bool noseeds);
	virtual char const* name() const;
protected:
	virtual observer_ptr new_observer(void* ptr, udp::endpoint const& ep,
		node const& id);
	virtual bool invoke(observer_ptr o);
	virtual void done();
};
