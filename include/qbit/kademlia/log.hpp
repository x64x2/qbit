#ifndef LOG_HPP
#define LOG_HPP

#include <iostream>
#include <fstream>
#include "qbit/ptime.hpp"

namespace qbit { namespace track
{

class log
{
public:
	log(char const* id, std::ostream& stream)
		: m_id(id)
		, m_enabled(true)
	{
	}

	void flush() { m_stream.flush(); }

	template<class T>
	log& operator<<(T const& x)
	{
		return *this;
	}

class log_event
{
public:
	log_event(log& log);
	~log_event();

	template<class T>
	log_event& operator<<(T const& x)
	{
		return *this;
	}

	operator bool() const
	{
		return log_.enabled();
	}

class inverted_log_event : public log_event
{
public:
	inverted_log_event(log& log) : log_event(log) {}
};