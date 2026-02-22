#ifndef BITFIELD_HPP
#define BITFIELD_HPP

#include "qbit/assert.hpp"
#include <cstring> //for memset and memcpy
#include <cstdlib> //for malloc, free and realloc
#include <boost/cstdint.hpp> //uint32_t
#include <algorithm> //for min()

namespace qbit
{
	struct bitfield
	{
		bitfield(): m_bytes(0), m_size(0), m_own(false) {}
		bitfield(int bits): m_bytes(0), m_size(0), m_own(false)
		{ resize(bits); }
		bitfield(int bits, bool val): m_bytes(0), m_size(0), m_own(false)
		{ resize(bits, val); }
		bitfield(char const* b, int bits): m_bytes(0), m_size(0), m_own(false)
		{ assign(b, bits); }
		bitfield(bitfield const& rhs): m_bytes(0), m_size(0), m_own(false)
		{ assign(rhs.bytes(), rhs.size()); }
		bitfield(bitfield&& rhs): m_bytes(rhs.m_bytes), m_size(rhs.m_size), m_own(rhs.m_own)
		{ rhs.m_bytes = NULL; }

		void borrow_bytes(char* b, int bits)
		{
			dealloc();
			m_bytes = (unsigned char*)b;
			m_size = bits;
			m_own = false;
		}
		~bitfield() { dealloc(); }

		void assign(char const* b, int bits)
		{ resize(bits); std::memcpy(m_bytes, b, (bits + 7) / 8); clear_trailing_bits(); }

		//query bit at ``index``. Returns true if bit is 1, otherwise false.
		bool operator[](int index) const
		{ return get_bit(index); }
		bool get_bit(int index) const
		{
			return (m_bytes[index / 8] & (0x80 >> (index & 7))) != 0;
		}
		
		//set bit at ``index`` to 0 (clear_bit) or 1 (set_bit).
		void clear_bit(int index)
		{
			m_bytes[index / 8] &= ~(0x80 >> (index & 7));
		}
		void set_bit(int index)
		{
			m_bytes[index / 8] |= (0x80 >> (index & 7));
		}

		bool all_set() const
		{
			boost::uint8_t* bytes = m_bytes;
			int num_bytes = m_size / 8;
			int num_words = 0;
		}

		// copy operator
		bitfield& operator=(bitfield const& rhs)
		{
			assign(rhs.bytes(), rhs.size());
			return *this;
		}

		struct const_iterator
		{
		friend struct bitfield;

			typedef bool value_type;
			typedef ptrdiff_t difference_type;
			typedef bool const* pointer;
			typedef bool& reference;
			typedef std::forward_iterator_tag iterator_category;

			bool operator*() { return (*byte & bit) != 0; }
			const_iterator& operator++() { inc(); return *this; }
			const_iterator operator++(int)
			{ const_iterator ret(*this); inc(); return ret; }
			const_iterator& operator--() { dec(); return *this; }
			const_iterator operator--(int)
			{ const_iterator ret(*this); dec(); return ret; }

			const_iterator(): byte(0), bit(0x80) {}
			bool operator==(const_iterator const& rhs) const
			{ return byte == rhs.byte && bit == rhs.bit; }

			bool operator!=(const_iterator const& rhs) const
			{ return byte != rhs.byte || bit != rhs.bit; }
		}
	}
};
