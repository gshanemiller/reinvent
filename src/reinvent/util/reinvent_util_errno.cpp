#include <util/reinvent_util_errno.h>

#include <vector>

namespace Reinvent {

const char * Util::Errno::d_missingFormatSpecifier = "<internal-error: format specifier missing>";

const std::vector<const char *> Util::Errno::d_formatSpecifier = {
  "%06u.%09u %s %s:%d Object '%s' not found\n",                                         // Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND
  "%06u.%09u %s %s:%d Cannot convert string '%s' to integer\n",                         // Util::Errno::REINVENT_UTIL_ERRNO_ATOI
  "%06u.%09u %s %s:%d Cannot convert string '%s' to boolean\n",                         // Util::Errno::REINVENT_UTIL_ERRNO_ATOB
  "%06u.%09u %s %s:%d '%s' %s %d does not satisfy required bounds [%d, %d]\n",          // Util::Errno::REINVENT_UTIL_ERRNO_RANGE
  "%06u.%09u %s %s:%d '%s' '%s' %s %d does not satisfy required bounds [%u, %u]\n",     // Util::Errno::REINVENT_UTIL_ERRNO_RANGE_LIST
  "%06u.%09u %s %s:%d '%s' '%s' %s %lu does not satisfy required bounds [%u, %u]\n",    // Util::Errno::REINVENT_UTIL_ERRNO_STRING_LENGTH
  "%06u.%09u %s %s:%d UNIX environment variable '%s' with value '%s' %s\n",             // Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT
  "%06u.%09u %s %s:%d Insufficient resources: requested %s %d but only %d %s\n",        // Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE
  "%06u.%09u %s %s:%d UNIX or DPDK API failure: %s: %s (errno=%d)\n"                    // Util::Errno::REINVENT_UTIL_ERRNO_API
};

const char * Util::Errno::format(const int reinvent_util_error_rcode) {
  const int base(static_cast<int>(Util::Errno::REINVENT_UTIL_ERRNO_BASE));
  const std::size_t index = reinvent_util_error_rcode - base - 1;
  return (index>=d_formatSpecifier.size()) ? d_missingFormatSpecifier : d_formatSpecifier[index];
}

} // namespace Reinvent
