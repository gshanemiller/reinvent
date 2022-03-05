#include <util/reinvent_util_errno.h>

#include <vector>

namespace Reinvent {

const char * Util::Errno::d_missingFormatSpecifier = "<internal-error: format specifier missing>";

const std::vector<const char *> Util::Errno::d_formatSpecifier = {
  "Object '%s' not found",                                              // Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND
  "Cannot convert string '%s' to integer",                              // Util::Errno::REINVENT_UTIL_ERRNO_ATOI
  "Cannot convert string '%s' to boolean",                              // Util::Errno::REINVENT_UTIL_ERRNO_ATOB
  "%s %s %d does not satisfy required bounds [%d, %d]",                 // Util::Errno::REINVENT_UTIL_ERRNO_RANGE
  "%s '%s' %s %d which does not satisfy required bounds [%u, %u]",      // Util::Errno::REINVENT_UTIL_ERRNO_RANGE_LIST
  "%s '%s' %s %lu does not satisfy required bounds [%u, %u]",           // Util::Errno::REINVENT_UTIL_ERRNO_STRING_LENGTH
  "UNIX environment variable '%s' with value '%s' %s",                  // Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT
  "Insufficient resources: requested %s %d but only %d %s",             // Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE
  "UNIX or DPDK API failure: %s: %s (errno=%d)"                         // Util::Errno::REINVENT_UTIL_ERROR_API
};

const char * Util::Errno::specifier(const int reinvent_util_error_rcode) {
  const int base(static_cast<int>(Util::Errno::REINVENT_UTIL_ERRNO_BASE));
  const std::size_t index = reinvent_util_error_rcode - base - 1;
  return (index>=d_formatSpecifier.size()) ? d_missingFormatSpecifier : d_formatSpecifier[index];
}

} // namespace Reinvent
