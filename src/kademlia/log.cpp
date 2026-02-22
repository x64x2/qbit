#include "qbit/kademlia/log.hpp"
#include "qbit/time.hpp"

namespace qbit { namespace track
{
	log_event::log_event(log& log) 
		: log_(log) 
	{
		if (log_.enabled())
			log_ << time_now_string() << " [" << log.id() << "] ";
	}

	log_event::~log_event()
	{
		if (log_.enabled())
		{
			log_ << "\n";
			log_.flush();
		}
	}

}}

