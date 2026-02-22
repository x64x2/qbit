#include <string>
#include <cassert>
#define ASSERT_PRECOND(x) assert(x)
#define ASSERT(x) assert(x)
#define ASSERT_VAL(x, y) assert(x)
#define ASSERT_PRECOND(a) do {} while(false)
#define ASSERT(a) do {} while(false)
#define ASSERT_VAL(a, b) do {} while(false)

std::string demangle(char const* name);
void print_backtrace(char* out, int len, int max_depth = 0);
extern char const* qbit_assert;


#if (defined __linux__ || defined __MACH__) && defined __GNUC__ && !USE_SYSTEM_ASSERT
#if USE_IOSTREAM
#include <sstream>

void assert_fail(const char* expr, int line, char const* file
	, char const* function, char const* val, int kind = 0);
  
  do { if (x) {} else assert_fail(#x, __LINE__, __FILE__, __PRETTY_FUNCTION__, "", 1); } while (false)
	do { if (x) {} else assert_fail(#x, __LINE__, __FILE__, __PRETTY_FUNCTION__, "", 0); } while (false)
	do { if (x) {} else { std::stringstream __s__; __s__ << #y ": " << y; \
	assert_fail(#x, __LINE__, __FILE__, __PRETTY_FUNCTION__, __s__.str().c_str(), 0); } } while (false)




