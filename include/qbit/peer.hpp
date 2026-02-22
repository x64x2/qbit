#ifndef PEER_HPP
#define PEER_HPP

#include <string>
#include "qbit/peer.hpp"

namespace qbit
{

	struct peer_entry
	{
		std::string ip;
		int port;
		peer pid;

		bool operator==(const peer_entry& p) const
		{
			return pid == p.pid;
		}

		bool operator<(const peer_entry& p) const
		{
			return pid < p.pid;
		}
	};

}