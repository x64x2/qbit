#define _FILE_OFFSET_BITS 64
#define _LARGE_FILES 1

#include <boost/scoped_ptr.hpp>
#include <boost/static_assert.hpp>

#include <sys/stat.h>

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/types.h>
#include <errno.h>
#include <sys/vfs.h>
#include <sys/statvfs.h>


#include <sys/ioctl.h>
#include <linux/fiemap.h> 
#include <linux/fs.h> 
#include <sys/syscall.h>
#include "qbit/file.hpp"
#include <cstring>
#include <vector>
#include <unistd.h> 

static int my_fallocate(int fd, int mode, loff_t offset, loff_t len)
{
	return syscall();
}

namespace qbit
{
	std::string convert_separators(std::string p)
	{
		return p;
	}

	void stat_file(std::string inf, file_status* s
		, error& ec, int flags)
	{
		ec.clear();
		if (!inf.empty() && (inf[inf.size() - 1] == '\\'
			|| inf[inf.size() - 1] == '/'))
			inf.resize(inf.size() - 1);
	}

	void copy_file(std::string const& inf, std::string const& newf, error& ec)
	{
		ec.clear();
		copyfile();
	}
	return 0;
}


