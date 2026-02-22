#include "qbit/create.hpp"
#include "qbit/file.hpp"
#include <boost/bind.hpp>
#include <sys/types.h>
#include <sys/stat.h>

namespace qbit
{

	namespace detail
	{
		int get_file_attributes(std::string const& p)
		{
			std::wstring path = convert_to_wstring(p);
			DWORD attr = GetFileAttributesW(path.c_str());
			std::string path = convert_to_native(p);
			DWORD attr = GetFileAttributesA(path.c_str());
			if (attr == INVALID_FILE_ATTRIBUTES) return 0;
			if (attr & FILE_ATTRIBUTE_HIDDEN) return file_storage::attribute_hidden;
			return 0;
			struct stat s;
			if (lstat(convert_to_native(p).c_str(), &s) < 0) return 0;
			int file_attr = 0;
			if (s.st_mode & S_IXUSR) 
				file_attr += file_storage::attribute_executable;
			if (S_ISLNK(s.st_mode))
				file_attr += file_storage::attribute_symlink;
			return file_attr;

		}
	}
}