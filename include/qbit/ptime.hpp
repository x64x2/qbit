#ifndef PTIME_HPP
#define PTIME_HPP

#include <string>
#include <boost/assert.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp

#include <boost/cstdint.hpp>

namespace qbit
{
	struct time_duration
	{
		time_duration() {}
		time_duration operator/(int rhs) const { return time_duration(diff / rhs); }
		explicit time_duration(boost::int64_t d) : diff(d) {}
		time_duration& operator-=(time_duration const& c)
		{ diff -= c.diff; return *this; }
		time_duration& operator+=(time_duration const& c)
		{ diff += c.diff; return *this; }
		time_duration& operator*=(int v) { diff *= v; return *this; }
		time_duration operator+(time_duration const& c)
		{ return time_duration(diff + c.diff); }
		time_duration operator-(time_duration const& c)
		{ return time_duration(diff - c.diff); }

		boost::int64_t diff;
	};
	struct ptime
	{
		ptime() {}
		explicit ptime(boost::uint64_t t): time(t) {}

		ptime& operator+=(time_duration rhs) { time += rhs.diff; return *this; }
		ptime& operator-=(time_duration rhs) { time -= rhs.diff; return *this; }

		boost::uint64_t time;
	};
	inline bool is_negative(time_duration dt) { return dt.diff < 0; }
	inline bool operator>(ptime lhs, ptime rhs)
	{ return lhs.time > rhs.time; }
	inline bool operator>=(ptime lhs, ptime rhs)
	{ return lhs.time >= rhs.time; }
	inline bool operator<=(ptime lhs, ptime rhs)
	{ return lhs.time <= rhs.time; }
	inline bool operator<(ptime lhs, ptime rhs)
	{ return lhs.time < rhs.time; }
	inline bool operator!=(ptime lhs, ptime rhs)
	{ return lhs.time != rhs.time;}
	inline bool operator==(ptime lhs, ptime rhs)
	{ return lhs.time == rhs.time;}
	inline bool operator==(time_duration lhs, time_duration rhs)
	{ return lhs.diff == rhs.diff; }
	inline bool operator<(time_duration lhs, time_duration rhs)
	{ return lhs.diff < rhs.diff; }
	inline bool operator<=(time_duration lhs, time_duration rhs)
	{ return lhs.diff <= rhs.diff; }
	inline bool operator>(time_duration lhs, time_duration rhs)
	{ return lhs.diff > rhs.diff; }
	inline bool operator>=(time_duration lhs, time_duration rhs)
	{ return lhs.diff >= rhs.diff; }
	inline time_duration operator*(time_duration lhs, int rhs)
	{ return time_duration(boost::int64_t(lhs.diff * rhs)); }
	inline time_duration operator*(int lhs, time_duration rhs)
	{ return time_duration(boost::int64_t(lhs * rhs.diff)); }
	inline time_duration operator-(ptime lhs, ptime rhs)
	{ return time_duration(lhs.time - rhs.time); }
	inline ptime operator+(ptime lhs, time_duration rhs)
	{ return ptime(lhs.time + rhs.diff); }
	inline ptime operator+(time_duration lhs, ptime rhs)
	{ return ptime(rhs.time + lhs.diff); }
	inline ptime operator-(ptime lhs, time_duration rhs)
	{ return ptime(lhs.time - rhs.diff); }

}




