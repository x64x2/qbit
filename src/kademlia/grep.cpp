#include <qbit/kademlia/grep.hpp>
#include <qbit/kademlia/route.hpp>
#include <qbit/kademlia/rpc.hpp>
#include <qbit/kademlia/node.hpp>
#include <qbit/io.hpp>
#include <qbit/socket.hpp>
#include <qbit/socket.hpp>
#include <vector>

namespace qbit 
{
using detail::read_endpoint_list;
using detail::read_v4_endpoint;
using detail::read_v6_endpoint;
}

namespace track {
void grep_observer::reply(msg const& m)
{
	if (!r)
	{
		return;
	}

	if (!id || id->string_length() != 20)
	{
		return 0;
	}
	if (token)
	{
		static_cast<grep*>(m_algorithm.get())->got_write_token(
			node(id->string_ptr()), token->string_value());
	}
	done();
}

void add_entry_fun(void* userdata, node_entry const& e)
{
	f->add_entry(e.id, e.ep(), observer::flag_initial);
}
}