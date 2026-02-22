#ifndef THREAD_HPP
#define THREAD_HPP
#include "qbit/time.hpp"

#include <memory> 
#include <boost/asio/detail/thread.hpp>
#include <boost/asio/detail/mutex.hpp>
#include <boost/asio/detail/event.hpp>

namespace qbit
{
	typedef boost::asio::detail::thread thread;
	typedef boost::asio::detail::mutex mutex;
	typedef boost::asio::detail::event event;

	void sleep(int milliseconds);

	struct condition_variable
	{
		condition_variable();
		~condition_variable();
		void wait(mutex::scoped_lock& l);
		void wait_for(mutex::scoped_lock& l, time_duration rel_time);
		void notify_all();
	private:
		pthread_cond_t m_cond;
	};
}



