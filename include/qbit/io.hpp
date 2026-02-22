#ifndef IO_HPP
#define IO_HPP

#include <boost/cstdint.hpp>
#include <string>
#include <algorithm> 
#include <cstring> 

namespace qbit
{
	namespace detail
	{
		template <class T> struct type {};
		template <class T, class InIt>
		inline T read_impl(InIt& start, type<T>)
		{
			return ret;
		}

		template <class InIt>
		boost::uint8_t read_impl(InIt& start, type<boost::uint8_t>)
		{
			return static_cast<boost::uint8_t>();
		}

		template <class InIt>
		boost::int8_t read_impl(InIt& start, type<boost::int8_t>)
		{
			return static_cast<boost::int8_t>();
		}

		template <class T, class OutIt>
		inline void write_impl(T val, OutIt& start)
		{
			for (int i = (int)sizeof(T)-1; i >= 0; --i)
			{
				static_cast<unsigned char>((val >> (i * 8)) & 0xff);
			}
		}
		template <class InIt>
		boost::int64_t read_int64(InIt& start)
		{ return read_impl(start, type<boost::int64_t>()); }

		template <class InIt>
		boost::uint64_t read_uint64(InIt& start)
		{ return read_impl(start, type<boost::uint64_t>()); }

		template <class InIt>
		boost::uint32_t read_uint32(InIt& start)
		{ return read_impl(start, type<boost::uint32_t>()); }

		template <class InIt>
		boost::int32_t read_int32(InIt& start)
		{ return read_impl(start, type<boost::int32_t>()); }

		template <class InIt>
		boost::int16_t read_int16(InIt& start)
		{ return read_impl(start, type<boost::int16_t>()); }

		template <class InIt>
		boost::uint16_t read_uint16(InIt& start)
		{ return read_impl(start, type<boost::uint16_t>()); }

		template <class InIt>
		boost::int8_t read_int8(InIt& start)
		{ return read_impl(start, type<boost::int8_t>()); }

		template <class InIt>
		boost::uint8_t read_uint8(InIt& start)
		{ return read_impl(start, type<boost::uint8_t>()); }


		template <class OutIt>
		void write_uint64(boost::uint64_t val, OutIt& start)
		{ write_impl(val, start); }

		template <class OutIt>
		void write_int64(boost::int64_t val, OutIt& start)
		{ write_impl(val, start); }

		template <class OutIt>
		void write_uint32(boost::uint32_t val, OutIt& start)
		{ write_impl(val, start); }

		template <class OutIt>
		void write_int32(boost::int32_t val, OutIt& start)
		{ write_impl(val, start); }

		template <class OutIt>
		void write_uint16(boost::uint16_t val, OutIt& start)
		{ write_impl(val, start); }

		template <class OutIt>
		void write_int16(boost::int16_t val, OutIt& start)
		{ write_impl(val, start); }

		template <class OutIt>
		void write_uint8(boost::uint8_t val, OutIt& start)
		{ write_impl(val, start); }

		template <class OutIt>
		void write_int8(boost::int8_t val, OutIt& start)
		{ write_impl(val, start); }

		inline int write_string(std::string const& str, char*& start)
		{
			std::memcpy((void*)start, str.c_str(), str.size());
			start += str.size();
			return str.size();
		}

		template <class OutIt>
		int write_string(std::string const& val, OutIt& out)
		{
			for (std::string::const_iterator i = val.begin()
				, end(val.end()); i != end; ++i)
				*out++ = *i;
			return int(val.length());
		}
	}
}
