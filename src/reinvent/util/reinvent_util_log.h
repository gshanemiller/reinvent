#pragma once

// Purpose: Logger
//
// Classes:
//   Util::LogRuntime: Controls log line format and filters log output with runtime severity limit`
// Macros:
//  REINVENT_UTIL_LOG_FATAL       - log at FATAL severity via streaming
//  REINVENT_UTIL_LOG_ERROR       - log at ERROR severity via streaming
//  REINVENT_UTIL_LOG_WARN        - log at WARN severity via streaming
//  REINVENT_UTIL_LOG_INFO        - log at INFO severity via streaming
//  REINVENT_UTIL_LOG_DEBUG       - log at DEBUG severity via streaming
//  REINVENT_UTIL_LOG_TRACE       - log at TRACE severity via streaming
//  REINVENT_UTIL_LOG_FATAL_VARGS - log at FATAL severity using printf style arguments
//  REINVENT_UTIL_LOG_ERROR_VARGS - log at ERROR severity using printf style arguments
//  REINVENT_UTIL_LOG_WARN_VARGS  - log at WARN severity using printf style arguments
//  REINVENT_UTIL_LOG_INFO_VARGS  - log at INFO severity using printf style arguments
//  REINVENT_UTIL_LOG_DEBUG_VARGS - log at DEBUG severity using printf style arguments
//  REINVENT_UTIL_LOG_TRACE_VARGS - log at TRACE severity using printf style arguments
// Macro Definitions:
//  REINVENT_UTIL_LOGGING_ON             - Must define on at build time to enable logging
//  REINVENT_UTIL_LOGGING_BUILD_SEVERITY - Defaults to REINVENT_UTIL_LOGGING_SEVERITY_WARN if not set in build.
//                                         Configures out of build all SEVERITY (see defines below) logging at any
//                                         higer severity. The remaining log calls are filtered at runtime. See
//                                         'LogRuntime::severityLimit()'
//  REINVENT_UTIL_LOGGING_TEST_FORMAT    - Define on at build time force-set the timestamp to constant 0
//  REINVENT_UTIL_LOGGING_SHORT_FILENAME - Define on at built time to log sub-string after last '/' token in
//                                       - __FILE__ which usually logs filename only not complete path
//
// Thread Safety: Not MT thread-safe. There is a race condition reading/writing the runtime severity limit. It is not
// mutex protected, however, it's just a integer. There are no deep concurrency problems there. Second, there is no
// guarantee the logged objects (which could be non-const) are not being changed in other threads.  But, again, they
// are likely value-semantic objects unavailable unaccessible in other threads. In sum logging is practically MT-safe.
//
// Exception Policy: No exceptions
//
// Description: Logging behavior is controlled at build time and runtime. First, if the preprocessor variable           
// 'REINVENT_UTIL_LOGGING_ON' is not defined at build time, all logging is excluded from any resulting object files. It 
// is defined out. Second, the preprocessor variable 'REINVENT_UTIL_LOGGING_BUILD_SEVERITY=<level>' excludes logging
// from object files at any severity higher than '<level>'. Whatever logging invocations remain in object code, are
// filtered at runtime by comparing the log severity of the invocation to a variable holding the current runtime
// severity limit. The logging format is controlled at build time only in code only; see 'FORMAT'. The default format
// is:
//
//   <secs-since-epoch>.<ns-since-epoch> <severity> <file>:<lineNumber> <caller-log-contents>
//
// Epoch is initialized to 0 by this library. 'clock_gettime' is used to get the current time so all timestamps will
// measure time elapsed since 'clock_gettime' epoch of Thursday, January 1, 1970 12:00:00 AM UTC by default. Therefore
// most callers run 'Util::LogRuntime::resetEpoch()' sometime before logging commences to get elapsed time since
// program initialization.
//
// All logs are written to 'stderr' and flushed.
//
// Macros are provided in streaming and printf (va_args) modes. Therefore any object with a free 'operator<<'
// 'std::ostream& operator<<(std::ostream& stream, const TYPE& object)' is eligible for stream based logging.
//
// Example:
//
//  Assume: built -DREINVENT_UTIL_LOGGING_ON -DREINVENT_UTIL_LOGGING_BUILD_SEVERITY==TRACE
//
//  {
//    Reinvent::Util::LogRuntime::resetEpoch();
//    Reinvent::Util::LogRuntime::setSeverityLimit(REINVENT_UTIL_LOGGING_SEVERITY_TRACE);
//
//    // Declare printf data to log
//    const int aInt(10);
//    const char *aStr="printf I/O log ";
//    const char *format="%s (rc=%d)\n";
//
//    // Printf based macros 
//    REINVENT_UTIL_LOG_FATAL_VARGS(format, aStr, aInt);
//    REINVENT_UTIL_LOG_ERROR_VARGS(format, aStr, aInt);
//    REINVENT_UTIL_LOG_WARN_VARGS(format, aStr, aInt);
//    REINVENT_UTIL_LOG_INFO_VARGS(format, aStr, aInt);
//    REINVENT_UTIL_LOG_DEBUG_VARGS(format, aStr, aInt);
//    REINVENT_UTIL_LOG_TRACE_VARGS(format, aStr, aInt);
//
//    // Declare stream data to log
//    const float bFloat(1.78);
//    const char *bStr="stream based I/O log ";
//    std::string bInfo(" (errno: 10)");
//
//    // Stream based macros
//    REINVENT_UTIL_LOG_FATAL(bStr << bFloat << bInfo << std::endl);
//    REINVENT_UTIL_LOG_ERROR(bStr << bFloat << bInfo << std::endl);
//    REINVENT_UTIL_LOG_WARN(bStr << bFloat << bInfo << std::endl);
//    REINVENT_UTIL_LOG_INFO(bStr << bFloat << bInfo << std::endl);
//    REINVENT_UTIL_LOG_DEBUG(bStr << bFloat << bInfo << std::endl);
//    REINVENT_UTIL_LOG_TRACE(bStr << bFloat << bInfo << std::endl);
//  }
// 
//  Will produce the output:
//
//  000000.000000086 FATAL test.cpp:28 printf I/O log  (rc=10)
//  000000.000025145 ERROR test.cpp:29 printf I/O log  (rc=10)
//  000000.000029039 WARN  test.cpp:30 printf I/O log  (rc=10)
//  000000.000036259 INFO  test.cpp:31 printf I/O log  (rc=10)
//  000000.000038583 DEBUG test.cpp:32 printf I/O log  (rc=10)
//  000000.000040651 TRACE test.cpp:33 printf I/O log  (rc=10)
//  000000.000044924 FATAL test.cpp:41 stream based I/O log 1.78 (errno: 10)
//  000000.000082503 TRACE test.cpp:42 stream based I/O log 1.78 (errno: 10)
//  000000.000117950 WARN  test.cpp:43 stream based I/O log 1.78 (errno: 10)
//  000000.000122083 INFO  test.cpp:44 stream based I/O log 1.78 (errno: 10)
//  000000.000124866 DEBUG test.cpp:45 stream based I/O log 1.78 (errno: 10)
//  000000.000129028 TRACE test.cpp:46 stream based I/O log 1.78 (errno: 10)

#include <iostream>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

namespace Reinvent {
namespace Util {

//
// Define basic logging severity levels
//
#define REINVENT_UTIL_LOGGING_SEVERITY_FATAL 0
#define REINVENT_UTIL_LOGGING_SEVERITY_ERROR 1
#define REINVENT_UTIL_LOGGING_SEVERITY_WARN  2
#define REINVENT_UTIL_LOGGING_SEVERITY_INFO  3
#define REINVENT_UTIL_LOGGING_SEVERITY_DEBUG 4
#define REINVENT_UTIL_LOGGING_SEVERITY_TRACE 5

//
// If the build time logging severity not defined, default to WARN
//
#ifndef REINVENT_UTIL_LOGGING_BUILD_SEVERITY
#define REINVENT_UTIL_LOGGING_BUILD_SEVERITY REINVENT_UTIL_LOGGING_SEVERITY_WARN
#endif

//
// Runtime log severity level and related runtime data
//
class LogRuntime {
  // DATA
  static unsigned d_severityLimit;
    // The runtime severity limit '0<=d_severity<=REINVENT_UTIL_LOGGING_BUILD_SEVERITY'
  static struct timespec d_epoch;
    // The zero time or epoch relative to which logged timestamps are calculated. It is initialized zero. To log
    // time elapsed since the start of the program holding this library, invoke 'resetEpoch' in main.

public:
  static const char *ERRNO_TAG;
  static const char *FATAL_TAG;
  static const char *ERROR_TAG;
  static const char *WARN_TAG;
  static const char *INFO_TAG;
  static const char *DEBUG_TAG;
  static const char *TRACE_TAG;
    // Constant width 5-character human readable severity level descriptions
  static const char *FORMAT;
    // Printf style format constant-width: '<secs>.<ns> <severity> '
  const static int DEFAULT_FORMAT_SIZE;
    // With in characters including 0 terminator of 'FORMAT' result assuming 5-character severity tags
  static const clockid_t CLOCK_TYPE;
    // The type of clock used when calling 'clock_gettime'

  // ACCESSORS
  static unsigned severityLimit();
    // Return the attribute 'severityLimit' value

  // MANIPULATORS
  static int setSeverityLimit(unsigned level);
    // Return 0 if the severity limit was updated to specified 'level' and non-zero otherwise. The behavior is defined
    // if '0<=level<=REINVENT_UTIL_LOGGING_BUILD_SEVERITY'. At runtime, all logging calls at any severity higher than
    // 'level' will not be done.
  
  static void logTimestampAndSeverity(FILE *stream, const char *severity);
    // Write to specified 'stream' data formatted as 'FORMAT' representing a timestamp, a space, specified 'severity'
    // followed by a space. The timestamp is calculated via 'clock_gettime' representing the time difference between
    // 'd_epoch' and now written as '<secs>.<ns>'. Although callers can provided any zero-terminated string for
    // 'severity' most callers will provided one of the tags above e.g. 'INFO_TAG'.
   
  static std::ostream& streamTimestampAndSeverity(std::ostream& stream, const char *severity);
    // Write to specified 'stream' data formatted as 'FORMAT' representing a timestamp, a space, specified 'severity'
    // followed by a space. The timestamp is calculated via 'clock_gettime' representing the time difference between
    // 'd_epoch' and now written as '<secs>.<ns>'. Although callers can provided any zero-terminated string for
    // 'severity' most callers will provided one of the tags above e.g. 'INFO_TAG'. Function unconditionlly returns
    // provided 'stream'.
   
  static const char *filename(const char *filename);
    // If REINVENT_UTIL_LOGGING_SHORT_FILENAME is not defined, return specified 'filename'. Otherwie return a pointer
    // by looking for the last '/' in 'filename' and returning the substring of filename one past '/'. Since library
    // filenames include the namespace in which the file occurs, shorte filenames are not ambiguous, and shorter.

  static struct timespec epoch();
    // Return the 'epoch' attribute value

  static void resetEpoch();
    // Assign to 'epoch' attribute to the value returned by 'clock_gettime' with clock type 'CLOCK_TYPE'.
};

//
// If logging enabled at build time
//
#ifdef REINVENT_UTIL_LOGGING_ON

//
// Printf style logging macros. Fatal is always enabled. Note that caller always provides a terminating ';' which 
// follows the macro invocation. Also note a new line terminator is provided only if caller's format string includes
// it. The macros do not add line delimiters.
//
#define REINVENT_UTIL_LOG_FATAL_VARGS(...)                                                                        \
    Reinvent::Util::LogRuntime::logTimestampAndSeverity(stderr, Reinvent::Util::LogRuntime::FATAL_TAG);           \
    fprintf(stderr, "%s:%d ", Reinvent::Util::LogRuntime::filename(__FILE__), __LINE__);                          \
    fprintf(stderr, __VA_ARGS__);                                                                                 \
    fflush(stderr)

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_ERROR
#define REINVENT_UTIL_LOG_ERROR_VARGS(...)                                                                        \
    if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_ERROR,1)) {  \
      Reinvent::Util::LogRuntime::logTimestampAndSeverity(stderr, Reinvent::Util::LogRuntime::ERROR_TAG);         \
      fprintf(stderr, "%s:%d ", Reinvent::Util::LogRuntime::filename(__FILE__), __LINE__);                        \
      fprintf(stderr, __VA_ARGS__);                                                                               \
    }
#else
#define REINVENT_UTIL_LOG_ERROR_VARGS(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_WARN
#define REINVENT_UTIL_LOG_WARN_VARGS(...)                                                                         \
    if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_WARN,1)) {   \
      Reinvent::Util::LogRuntime::logTimestampAndSeverity(stderr, Reinvent::Util::LogRuntime::WARN_TAG);          \
      fprintf(stderr, "%s:%d ", Reinvent::Util::LogRuntime::filename(__FILE__), __LINE__);                        \
      fprintf(stderr, __VA_ARGS__);                                                                               \
    }
#else
#define REINVENT_UTIL_LOG_WARN_VARGS(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_INFO
#define REINVENT_UTIL_LOG_INFO_VARGS(...)                                                                         \
    if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_INFO,0)) {   \
      Reinvent::Util::LogRuntime::logTimestampAndSeverity(stderr, Reinvent::Util::LogRuntime::INFO_TAG);          \
      fprintf(stderr, "%s:%d ", Reinvent::Util::LogRuntime::filename(__FILE__), __LINE__);                        \
      fprintf(stderr, __VA_ARGS__);                                                                               \
    }
#else
#define REINVENT_UTIL_LOG_INFO_VARGS(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_DEBUG
#define REINVENT_UTIL_LOG_DEBUG_VARGS(...)                                                                        \
    if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_DEBUG,0)) {  \
      Reinvent::Util::LogRuntime::logTimestampAndSeverity(stderr, Reinvent::Util::LogRuntime::DEBUG_TAG);         \
      fprintf(stderr, "%s:%d ", Reinvent::Util::LogRuntime::filename(__FILE__), __LINE__);                        \
      fprintf(stderr, __VA_ARGS__);                                                                               \
    }
#else
#define REINVENT_UTIL_LOG_DEBUG_VARGS(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_TRACE
#define REINVENT_UTIL_LOG_TRACE_VARGS(...)                                                                        \
    if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_TRACE,0)) {  \
      Reinvent::Util::LogRuntime::logTimestampAndSeverity(stderr, Reinvent::Util::LogRuntime::TRACE_TAG);         \
      fprintf(stderr, "%s:%d ", Reinvent::Util::LogRuntime::filename(__FILE__), __LINE__);                        \
      fprintf(stderr, __VA_ARGS__);                                                                               \
    }
#else
#define REINVENT_UTIL_LOG_TRACE_VARGS(...) ((void)0)
#endif

//
// Stream style logging macros. Fatal is always enabled. Note that caller always provides a terminating ';' which 
// follows the macro invocation. Also note a new line terminator is provided only if caller's stream includes it. The
// macros do not add line delimiters. Fatal/error is written to cerr and is flushed. The rest of the methods log to
// cout and are not flushed.
//
#define REINVENT_UTIL_LOG_FATAL(REINVENT_UTIL_LOG_STREAM_ARGS)                                                    \
  Reinvent::Util::LogRuntime::streamTimestampAndSeverity(std::cerr, Reinvent::Util::LogRuntime::FATAL_TAG)        \
  << Reinvent::Util::LogRuntime::filename(__FILE__)                                                               \
  << ":"                                                                                                          \
  << __LINE__                                                                                                     \
  << " "                                                                                                          \
  << REINVENT_UTIL_LOG_STREAM_ARGS                                                                                \
  << std::flush

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_ERROR
#define REINVENT_UTIL_LOG_ERROR(REINVENT_UTIL_LOG_STREAM_ARGS)                                                    \
  if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_ERROR,1)) {    \
    Reinvent::Util::LogRuntime::streamTimestampAndSeverity(std::cerr, Reinvent::Util::LogRuntime::TRACE_TAG)      \
    << Reinvent::Util::LogRuntime::filename(__FILE__)                                                             \
    << ":"                                                                                                        \
    << __LINE__                                                                                                   \
    << " "                                                                                                        \
    << REINVENT_UTIL_LOG_STREAM_ARGS                                                                              \
    << std::flush;                                                                                                \
  }
#else
#define REINVENT_UTIL_LOG_ERROR_(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_WARN
#define REINVENT_UTIL_LOG_WARN(REINVENT_UTIL_LOG_STREAM_ARGS)                                                     \
  if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_WARN,1)) {     \
    Reinvent::Util::LogRuntime::streamTimestampAndSeverity(std::cerr, Reinvent::Util::LogRuntime::WARN_TAG)       \
    << Reinvent::Util::LogRuntime::filename(__FILE__)                                                             \
    << ":"                                                                                                        \
    << __LINE__                                                                                                   \
    << " "                                                                                                        \
    << REINVENT_UTIL_LOG_STREAM_ARGS;                                                                             \
  }
#else
#define REINVENT_UTIL_LOG_WARN(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_INFO
#define REINVENT_UTIL_LOG_INFO(REINVENT_UTIL_LOG_STREAM_ARGS)                                                     \
  if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_INFO,0)) {     \
    Reinvent::Util::LogRuntime::streamTimestampAndSeverity(std::cerr, Reinvent::Util::LogRuntime::INFO_TAG)       \
    << Reinvent::Util::LogRuntime::filename(__FILE__)                                                             \
    << ":"                                                                                                        \
    << __LINE__                                                                                                   \
    << " "                                                                                                        \
    << REINVENT_UTIL_LOG_STREAM_ARGS;                                                                             \
  }
#else
#define REINVENT_UTIL_LOG_INFO(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_DEBUG
#define REINVENT_UTIL_LOG_DEBUG(REINVENT_UTIL_LOG_STREAM_ARGS)                                                    \
  if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_DEBUG,0)) {    \
    Reinvent::Util::LogRuntime::streamTimestampAndSeverity(std::cerr, Reinvent::Util::LogRuntime::DEBUG_TAG)      \
    << Reinvent::Util::LogRuntime::filename(__FILE__)                                                             \
    << ":"                                                                                                        \
    << __LINE__                                                                                                   \
    << " "                                                                                                        \
    << REINVENT_UTIL_LOG_STREAM_ARGS;                                                                             \
  }
#else
#define REINVENT_UTIL_LOG_DEBUG(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_TRACE
#define REINVENT_UTIL_LOG_TRACE(REINVENT_UTIL_LOG_STREAM_ARGS)                                                    \
  if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_TRACE,0)) {    \
    Reinvent::Util::LogRuntime::streamTimestampAndSeverity(std::cerr, Reinvent::Util::LogRuntime::TRACE_TAG)      \
    << Reinvent::Util::LogRuntime::filename(__FILE__)                                                             \
    << ":"                                                                                                        \
    << __LINE__                                                                                                   \
    << " "                                                                                                        \
    << REINVENT_UTIL_LOG_STREAM_ARGS;                                                                             \
  }
#else
#define REINVENT_UTIL_LOG_TRACE(...) ((void)0)
#endif

#else

//
// Logging is disabled at build time. Define it out
//
#define REINVENT_UTIL_LOG_FATAL_VARGS(...)          ((void)0)
#define REINVENT_UTIL_LOG_ERROR_VARGS(...)          ((void)0)
#define REINVENT_UTIL_LOG_WARN_VARGS(...)           ((void)0)
#define REINVENT_UTIL_LOG_INFO_VARGS(...)           ((void)0)
#define REINVENT_UTIL_LOG_DEBUG_VARGS(...)          ((void)0)
#define REINVENT_UTIL_LOG_TRACE_VARGS(...)          ((void)0)

#define REINVENT_UTIL_LOG_FATAL(REINVENT_UTIL_LOG_STREAM_ARGS)  ((void)0)
#define REINVENT_UTIL_LOG_ERROR(REINVENT_UTIL_LOG_STREAM_ARGS)  ((void)0)
#define REINVENT_UTIL_LOG_WARN(REINVENT_UTIL_LOG_STREAM_ARGS)   ((void)0)
#define REINVENT_UTIL_LOG_INFO(REINVENT_UTIL_LOG_STREAM_ARGS)   ((void)0)
#define REINVENT_UTIL_LOG_DEBUG(REINVENT_UTIL_LOG_STREAM_ARGS)  ((void)0)
#define REINVENT_UTIL_LOG_TRACE(REINVENT_UTIL_LOG_STREAM_ARGS)  ((void)0)

#endif

} // namespace util
} // namespace reinvent
