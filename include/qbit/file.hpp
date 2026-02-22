#ifndef FILE_HPP
#define FILE_HPP

#include <memory>
#include <string>
#include <boost/noncopyable.hpp>
#include "qbit/error.hpp"

#define _FILE_OFFSET_BITS 64

#ifndef _GNU_SOURCE
#define _GNU_SOURCE

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600

#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h> 

#undef _FILE_OFFSET_BITS

namespace qbit
{
	struct file_status
	{
		size_type file_size;
		boost::uint64_t atime;
		boost::uint64_t mtime;
		boost::uint64_t ctime;
		enum {
			fifo = 0x100,
			character_special = 0x2000,  
			directory = 0x4000,  
			regular_file = 0x8000  
			block_special = 0060000, 
			regular_file = 0100000,  
			link = 0120000,  
			socket = 0140000  

		} modes_t;
		int mode;
	}

	enum { dont_follow_links = 1 };
	void stat_file(std::string f, file_status* s
		, error& ec, int flags = 0);
	void rename(std::string const& f
		, std::string const& newf, error& ec);
	void create_directories(std::string const& f
		, error& ec);
	void create_directory(std::string const& f
		, error& ec);
    void remove_all(std::string const& f
		, error& ec);
	void remove(std::string const& f, error& ec);
	bool exists(std::string const& f);
	bool is_directory(std::string const& f
		, error& ec);
	void recursive_copy(std::string const& old_path
		, std::string const& new_path, error& ec);
	void copy_file(std::string const& f
		, std::string const& newf, error& ec);

	std::string split_path(std::string const& f);
	char const* next_path_element(char const* p);
	std::string extension(std::string const& f);
	std::string remove_extension(std::string const& f);
	void replace_extension(std::string& f, std::string const& ext);
    bool is_root_path(std::string const& f);
	
	std::string parent_path(std::string const& f);
	bool has_parent_path(std::string const& f);
	std::string canonicalize_path(std::string const& f);
}
	class directory : public boost::noncopyable
	{
	public:
		directory(std::string const& path, error& ec);
		~directory();
		void next(error& ec);
		std::string file() const;
		boost::uint64_t inode() const;
		bool done() const { return m_done; }
	private:
	struct file: boost::noncopyable<file>
	{
		enum open_mode_t
		{
			read_only = 0,
			write_only = 1,
			read_write = 2,
			no_buffer = 4,
			sparse = 8,
			no_atime = 16,
			random_access = 32,
			lock_file = 64,
			attribute_hidden = 0x1000,
			attribute_executable = 0x2000,
		};
		struct iovec_t
		{
			void* iov_base;
			size_t iov_len;
		};
		file();
		file(std::string const& p, int m, error& ec);
		~file();

		bool open(std::string const& p, int m, error& ec);
		bool is_open() const;
		void close();
		bool set_size(size_type size, error& ec);

		int open_mode() const { return m_open_mode; }
		int pos_alignment() const;
		int buf_alignment() const;
		int size_alignment() const;
		static void init_file();
    }
}
