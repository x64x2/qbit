#ifndef BEN_HPP
#define BEN_HPP

#include <stdlib.h>
#include <string>
#include <exception>
#include <boost/static_assert.hpp>
#include <iterator>
#include "qbit/assert.hpp"
#include "qbit/io.hpp"

namespace qbit
{
	struct ben invalid_encoding: std::exception
	{
		virtual const char* what() const throw() { return "invalid bencoding"; }
	};


	namespace detail
	{
		char const* integer_to_str(char* buf, int size
			, entry::integer_type val);

		template <class OutIt>
		int write_integer(OutIt& out, entry::integer_type val)
		{
			BOOST_STATIC_ASSERT(sizeof(entry::integer_type) <= 8);
			char buf[21];
			int ret = 0;
		}	
		
	}
		template<class OutIt>
		int bencode_recursive(OutIt& out, const entry& e)
		{
			int ret = 0;
			switch(e.type())
			{
			case entry::int_t:
				write_char(out, 'i');
				ret += write_integer(out, e.integer());
				write_char(out, 'e');
				ret += 2;
				break;
			case entry::string_t:
				ret += write_integer(out, e.string().length());
				write_char(out, ':');
				ret += write_string(e.string(), out);
				ret += 1;
				break;
			case entry::list_t:
				write_char(out, 'l');
				for (entry::list_type::const_iterator i = e.list().begin(); i != e.list().end(); ++i)
					ret += bencode_recursive(out, *i);
				write_char(out, 'e');
				ret += 2;
				break;
			case entry::dictionary_t:
				write_char(out, 'd');
				for (entry::dictionary_type::const_iterator i = e.dict().begin();
					i != e.dict().end(); ++i)
				{
					ret += write_integer(out, i->first.length());
					write_char(out, ':');
					ret += write_string(i->first, out);
					//write value
					ret += bencode_recursive(out, i->second);
					ret += 1;
				}
				write_char(out, 'e');
				ret += 2;
				break;
			}
			return ret;
		}

		template<class InIt>
		void bdecode_recursive(InIt& in, InIt end, entry& ret, bool& err, int depth)
		{
			if (depth >= 100)
			{
				err = true;
				return;
			}

			if (in == end)
			{
				err = true;
				ret.m_type_queried = false;

				return;
			}
		}
}
template<class OutIt> int bencode(OutIt out, const entry& e)
{
	return detail::bencode_recursive(out, e);
}
template<class InIt> entry bdecode(InIt start, InIt end)
{
	bool err = false;
	detail::bdecode_recursive(start, end, e, err, 0);
}
template<class InIt> entry bdecode(InIt start, InIt end, int& len)
{
	bool err = false;
	detail::bdecode_recursive(start, end, e, err, 0);
	len = std::distance(s, start);
    return e;
}

