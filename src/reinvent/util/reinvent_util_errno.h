#pragma once

// Purpose: Uniform error reporting
//
// Classes:
//   Util::Errno: Provide an error code set matched 1:1 to printf style for uniform error descriptions.
// Macros:
//   REINVENT_UTIL_ERRNO_RETURN - return an error value with optional assertion, logging
// Macro Definitions:
//   REINVENT_UTIL_ERRNO_LOGGING       - Must define on at build time in in addition to other logging defines to
//                                       produce errors at severity 'ERRNO'. This severity is not controlled in
//                                       Util::Log only by this definition
//   NDEBUG                            - If defined at build, as with any assert, the assert is stripped from build
//                                       Otherwise an assertion is produced at time when the asserted condition is
//                                       false
// 
// Thread Safety: Not MT safe. There is no guarantee that logged, or asserted objects are not being modified by another
// thread. Macros cannot restrict callers to const objects. However, since the macros only read provided values, and
// its improbable other threads know the address of those same values, the macros are practically MT-safe.
//
// Exception Policy: No exceptions
//
// Description: To faciliate the uniform error reporting this software component provides macros to name, assert,
// and return error codes. It eschews free form pigeon-dropping-logging code moving to a uniform standard where code
// method either return 0 on success or a non-zero code with matched printf-style description. This library does for
// reinvent what errno does the C-library.
//
// Wherever you'd write 'if (!condition) { return errorCode; }' write:
//
//    REINVENT_UTIL_ERRNO_RETURN(<errorEnum>, (condition), arg1, arg2, ...);
//
// where 'arg1, arg2, ...' are required printf-style values conforming to the printf-style format matched to the
// error enumerated value, and 'errorEnum' is one of the Errno enumerated values. The behavior of the macro is:
//
// (*) First, provided NDEBUG was not defined at build time, execute 'assert(condition)'. If the macro fails normal
//     assertion behavior occurs. That is, the program will abort without proceeding to the next bullet; Refer to the
//     man page '<assert.h>'. If the condition is true, there are no further actions. If NDEBUG is defined, the
//     assertion is stripped from the build.
// (*) Next, the macro tries to log the error at severity ERRNO that is, to see where and how errors bubble up in call
//     chains. If logging is disabled (see reinvent_util_log.h) at build time, or REINVENT_UTIL_LOGGING_ERRNO is not
//     defined at build time, the logging call is a no-op. ERRNO severity logs include both the error code both as a
//     symbol name and as an integer.
// (*) Finally, the macro returns the error code i.e. argument 1 to the macro call.
//
// The library convention is zero means success, and non-zero means error. As such the library uses the macros herein
// provide meaningful non-zero error codes and their description. Although there's slightly more book-keeping, the
// goals are intended the benefit library users not library developers:
//
// Default behavior is to not assert and emit no ERRNO logs
//
// (*) Encourage library developers to make contracts
// (*) Return codes from a predefined set each with a predefined printf-format resulting in uniform log lines
// (*) Eschew ad-hoc log error, verbose, debug noise where programmers drop in variously formatted fact-finding info
// (*) Encourage library developers to rely on error codes to harden code
// (*) Discourage library developers from using the log to debug problems; check the log isn't engineering. What'd
//     it return is.
// (*) To have a uniform, cohesive interplay between asserts, codes, and logging rather than sometimes and sometimes
//     no using some or all of those tools to trap errors function by function
// (*) Disable/enable the features desired at build time
// (*) Single source: error codes and their formats can only appear in this cpp/h file pair
// (*) Searchability
//
// Example:
//
// This example provides a method 'toInt' method wrapping 'strtol' returning Errno::ATO on error and zero otherwise:
// provided for you in this file; developers need not define it: 
//
//    #include <reinvent_util_errno.h>
//
//    int toInt(const char* stringValue, int *value, const char terminal) {                          
//      char *invalid(0);                                                                                             
//      // Cannot be empty
//      if (strlen(stringValue)) {                                                                                      
//        *value = strtol(stringValue, &invalid, 10);                                                                   
//        if (invalid && *invalid==terminal) {
//          return 0; // success
//        }
//      }
//      REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::ATOI, (invalid && *invalid==terminal), stringValue);                
//    }
//
//    int main() {  
//      int aInt(0);
//      const char *goodInt = "-100";
//      const char *badInt = "--100";
//      assert(toInt(goodInt, &aInt, 0)==0 && aInt==-100);
//      int rc = toInt(badInt, &aInt, 0);
//      printf("badCase returns rc==%d\n", rc);
//      return 0;
//    }
//
// In the good case there is no assetion, error, and nothing to log no matter how built. 'toInt' returns 0. In the
// bad case the extra '-' is a problem. How that problem is reported at runtime depends on how the code is built.
// 
// The runtime behavior of this code comes in three variations. The first variation is when assertion checking is ON;
// that is when the code is built ommiting -DNDEBUG. Since the assert is hit, logging never occurs because the task
// aborts then and there:
//
//    $ ./test
//    Assertion failed: (((invalid && *invalid==terminal))), function toInt, file test.cpp, line 10.
//
// The second variation is when assertion testing is disabled but logging is left ON; that is when build with -DNDEBUG
// and -DREINVENT_UTIL_LOGGING_ERRNO and -DREINVENT_UTIL_LOGGING_ON:
//
//    $ ./test
//    000000000.000045 ERRNO test.cpp:10 Cannot convert string '--100' to integer (Errno::ATOI=10001)
//    badCase returns rc==10001
//
// Finally, assertion checking and logging can be off:
//
//    $ ./test
//    badCase returns rc==10001

#include <util/reinvent_util_log.h>

#include <vector>

#include <assert.h>

namespace Reinvent {
namespace Util {

struct Errno {
  // ENUM
  enum {
    REINVENT_UTIL_ERRNO_BASE = 100000,    // UNIX errno values bound from below
    REINVENT_UTIL_ERRNO_NOTFOUND,         // N.B. DPDK errno values are negative
    REINVENT_UTIL_ERRNO_ATOI,
    REINVENT_UTIL_ERRNO_ATOB,
    REINVENT_UTIL_ERRNO_RANGE,
    REINVENT_UTIL_ERRNO_RANGE_LIST,       // 100005
    REINVENT_UTIL_ERRNO_STRING_LENGTH,
    REINVENT_UTIL_ERRNO_ENVIRONMENT,
    REINVENT_UTIL_ERRNO_NO_RESOURCE,
    REINVENT_UTIL_ERRNO_API,              // 100009
  };
  // DATA
  static const char * d_missingFormatSpecifier;
  static const std::vector<const char *> d_formatSpecifier;
  // ACCESSORS
  static const char * specifier(const int reinvent_util_error_rcode);
};

#ifndef NDEBUG
#ifdef REINVENT_UTIL_LOGGING_ON
#ifdef REINVENT_UTIL_LOGGING_ERRNO
#define REINVENT_UTIL_ERRNO_RETURN(REINVENT_UTIL_ERRNO_RETURN_SYMBOL, REINVENT_UTIL_ERRNO_RETURN_EXPR, ...)       \
  assert((REINVENT_UTIL_ERRNO_RETURN_EXPR));                                                                      \
  Reinvent::Util::LogRuntime::logTimestampAndSeverity(stderr, Reinvent::Util::LogRuntime::ERRNO_TAG);             \
  fprintf(stderr, "%s:%d ", Reinvent::Util::LogRuntime::filename(__FILE__), __LINE__);                            \
  fprintf(stderr, Reinvent::Util::Errno::specifier(REINVENT_UTIL_ERRNO_RETURN_SYMBOL), __VA_ARGS__);              \
  fprintf(stderr, " ("#REINVENT_UTIL_ERRNO_RETURN_SYMBOL"=%d)\n", REINVENT_UTIL_ERRNO_RETURN_SYMBOL);             \
  fflush(stderr);                                                                                                 \
  return (REINVENT_UTIL_ERRNO_RETURN_SYMBOL)
#endif
#endif
#endif

#ifdef NDEBUG
#ifdef REINVENT_UTIL_LOGGING_ON
#ifdef REINVENT_UTIL_LOGGING_ERRNO
#define REINVENT_UTIL_ERRNO_RETURN(REINVENT_UTIL_ERRNO_RETURN_SYMBOL, REINVENT_UTIL_ERRNO_RETURN_EXPR, ...)       \
  Reinvent::Util::LogRuntime::logTimestampAndSeverity(stderr, Reinvent::Util::LogRuntime::ERRNO_TAG);             \
  fprintf(stderr, "%s:%d ", Reinvent::Util::LogRuntime::filename(__FILE__), __LINE__);                            \
  fprintf(stderr, Reinvent::Util::Errno::specifier(REINVENT_UTIL_ERRNO_RETURN_SYMBOL), __VA_ARGS__);              \
  fprintf(stderr, " ("#REINVENT_UTIL_ERRNO_RETURN_SYMBOL"=%d)\n", REINVENT_UTIL_ERRNO_RETURN_SYMBOL);             \
  fflush(stderr);                                                                                                 \
  return (REINVENT_UTIL_ERRNO_RETURN_SYMBOL)
#endif
#endif
#endif

// Default case

#ifndef REINVENT_UTIL_ERRNO_RETURN
#define REINVENT_UTIL_ERRNO_RETURN(REINVENT_UTIL_ERRNO_RETURN_SYMBOL, REINVENT_UTIL_ERRNO_RETURN_EXPR, ...)       \
  return (REINVENT_UTIL_ERRNO_RETURN_SYMBOL)
#endif

} // namespace Util 
} // namespace Reinvent 
