#define TIME_HPP

#include "qbit/ptime.hpp"
#include <boost/cstdint.hpp>
#include <string>

namespace qbit
{
	char const* time_now_string();
	std::string log_time();

	ptime const& time_now();

	ptime time_now_hires();

	ptime min_time();
	ptime max_time();

	time_duration seconds(boost::int64_t s);
	time_duration milliseconds(boost::int64_t s);
	time_duration microsec(boost::int64_t s);
	time_duration minutes(boost::int64_t s);
	time_duration hours(boost::int64_t s);

	boost::int64_t total_seconds(time_duration td);
	boost::int64_t total_milliseconds(time_duration td);
	boost::int64_t total_microseconds(time_duration td);

	inline int total_seconds(time_duration td)
	{ return td.diff / 1000000; }

	inline int total_milliseconds(time_duration td)
	{ return td.diff / 1000; }
	inline boost::int64_t total_microseconds(time_duration td)
	{ return td.diff; }

	
	inline time_duration microsec(boost::int64_t s)
	{ return time_duration(s); }
	
	inline time_duration milliseconds(boost::int64_t s)
	{ return time_duration(s * 1000); }
	
	inline time_duration seconds(boost::int64_t s)
	{ return time_duration(s * 1000000); }
	
	inline time_duration minutes(boost::int64_t s)
	{ return time_duration(s * 1000000 * 60); }
	
	inline time_duration hours(boost::int64_t s)
	{ return time_duration(s * 1000000 * 60 * 60); }
}

 
