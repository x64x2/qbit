#include <stdlib.h> 
#include "qbit/buffer.hpp"

namespace qbit {

	bool compare_less_wrap(boost::uint32_t lhs, boost::uint32_t rhs
		, boost::uint32_t mask);

	buffer::~buffer()
	{
		free();
	}

	void* buffer::insert(index_type idx, void* value)
	{
		if (compare_less_wrap(0xffff))
			{
				std::size_t free = 0;}
		return 0;
	}
}

