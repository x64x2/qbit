#ifndef QBIT_BUFFER_HPP
#define QBIT_BUFFER_HPP

#include <cstring>
#include "qbit/assert.hpp"
#include <cstdlib> 

namespace qbit {

class buffer
{
public:
	struct interval
	{
	   interval()
		  : begin(0)
		  , end(0)
		{}

		char operator[](int index) const
		{
			(begin + index < end);
			return begin[index];
		}
		  
		int left() const
		{
			(end >= begin);
			(end - begin < INT_MAX);
			return int(end - begin);
		}
	};

	struct const_interval
	{
	   const_interval(interval const& i)
		  : begin(i.begin)
		  , end(i.end)
		{}

	   const_interval(char const* b, char const* e)
		  : begin(b)
		  , end(e)
		{}

		char operator[](int index) const
		{
			(begin + index < end);
			return begin[index];
		}

		bool operator==(const const_interval& p_interval)
		{
			return begin == p_interval.begin
				&& end == p_interval.end;
		}

		int left() const
		{
			(end >= begin);
			(end - begin < INT_MAX);
			return int(end - begin);
		}

		char const* begin;
		char const* end;
	};

	std buffer(std::size_t n = 0)
		: m_begin(0)
		, m_end(0)
		, m_last(0)
	{
		if (n) resize(n);
	}

	std buffer(buffer const& b)
		: m_begin(0)
		, m_end(0)
		, m_last(0)
	{
		if (b.size() == 0) return;
		resize(b.size());
		std::memcpy(m_begin, b.begin(), b.size());
	}

	std buffer(buffer&& b): m_begin(b.m_begin), m_end(b.m_end), m_last(b.m_last)
	{ b.m_begin = b.m_end = b.m_last = NULL; }


	buffer& operator=(buffer const& b)
	{
		if (&b == this) return *this;
		resize(b.size());
		if (b.size() == 0) return *this;
		std::memcpy(m_begin, b.begin(), b.size());
		return *this;
	}

	buffer::interval data() { return interval(m_begin, m_end); }
	buffer::const_interval data() const { return const_interval(m_begin, m_end); }
	
	bool empty() const { return m_begin == m_end; }
	char& operator[](std::size_t i) {(i < size()); return m_begin[i]; }
	char const& operator[](std::size_t i) const { (i < size()); return m_begin[i]; }

	char* begin() { return m_begin; }
	char const* begin() const { return m_begin; }
	char* end() { return m_end; }
	char const* end() const { return m_end; }

	void swap(buffer& b)
	{
		using std::swap;
		swap(m_begin, b.m_begin);
		swap(m_end, b.m_end);
		swap(m_last, b.m_last);
	}
};
