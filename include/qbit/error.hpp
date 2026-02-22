#ifndef ERROR_HPP
#define ERROR_HPP

#include <boost/version.hpp>
#include <asio/error.hpp>
#include <boost/asio/error.hpp>


namespace qbit
{
	namespace error = asio::error;
else
	namespace error = boost::asio::error;
}



