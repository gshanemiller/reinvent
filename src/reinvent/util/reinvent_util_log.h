#pragma once

// Purpose: Logger
//
// Classes:
//   Util::LogRuntime: Controls log line format and filters log output with runtime severity limit
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
//  REINVENT_UTIL_LOG_CONCAT      - log helper macro
//  REINVENT_UTIL_LOG_VA_ARGS     - log helper macro
// Macro Definitions:
//  REINVENT_UTIL_LOGGING_ON             - Must define on at build time to enable logging
//  REINVENT_UTIL_LOGGING_BUILD_SEVERITY - Defaults to REINVENT_UTIL_LOGGING_SEVERITY_WARN if not set in build.
//                                         Configures out of build all SEVERITY (see defines below) logging at any
//                                         higher severity. The remaining log calls are filtered at runtime. See
//                                         'LogRuntime::severityLimit()'
//  REINVENT_UTIL_LOGGING_TEST_FORMAT    - Define on at build time to force-set the timestamp to constant 0
//  REINVENT_UTIL_LOGGING_SHORT_FILENAME - Define on at built time to log sub-string after last '/' token in
//                                       - __FILE__ which usually logs filename only not complete path
//
// Thread Safety: MT thread-safe.
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
// Macros are provided in streaming and printf (va_args) modes. Therefore any object with a free 'operator<<'
// 'std::ostream& operator<<(std::ostream& stream, const TYPE& object)' is eligible for stream based logging.
//
// Note: this is not performant code. Stream based macros use a mutex. The varargs macros do not use a mutex.

#include <iostream>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <mutex>

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

const int DEFAULT_FORMAT_SIZE = 24;
  // Width in characters including 0 terminator of 'FORMAT' result assuming 5-character severity tags

//
// Logger helper macro
//
#ifndef REINVENT_UTIL_LOG_CONCAT
#define REINVENT_UTIL_LOG_CONCAT(x, y) x y
#endif

#ifndef REINVENT_UTIL_LOG_VA_ARGS
#define REINVENT_UTIL_LOG_VA_ARGS(...) , ##__VA_ARGS__
#endif

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
  static const char *FORMAT;
    // Printf style format constant-width: '<secs>.<ns> <severity> '
  static const clockid_t CLOCK_TYPE;
    // The type of clock used when calling 'clock_gettime'
  static std::mutex lock;
    // Stream based logging requires sychronization

  // ACCESSORS
  static unsigned severityLimit();
    // Return the attribute 'severityLimit' value

  // MANIPULATORS
  static int setSeverityLimit(unsigned level);
    // Return 0 if the severity limit was updated to specified 'level' and non-zero otherwise. The behavior is defined
    // if '0<=level<=REINVENT_UTIL_LOGGING_BUILD_SEVERITY'. At runtime, all logging calls at any severity higher than
    // 'level' will not be done.
  
  static void elapsedTime(unsigned *sec, unsigned *ns);
    // Assign to specified 'sec, ns' the number of seconds and nanseconds that have elapsed since 'd_epoch'

  static void logTimestampAndSeverity(char *buf, const char *severity);
    // Write to specified 'buf' data formatted as 'FORMAT' representing a timestamp, a space, specified 'severity'
    // followed by a space. The timestamp is calculated via 'clock_gettime' representing the time difference between
    // 'd_epoch' and now written as '<secs>.<ns>' e.g. the time elapsed since the epoch. Although callers can provided
    // any zero-terminated string for 'severity' most callers will provide one of the tags above e.g. 'INFO_TAG'. The
    // behavior is defined if 'buf' is 'sizeof(buf)==DEFAULT_FORMAT_SIZE'.
   
  static const char *filename(const char *filename);
    // If REINVENT_UTIL_LOGGING_SHORT_FILENAME is not defined, return specified 'filename' unchanged. Otherwie return
    // a pointer looking for the last '/' in 'filename' and returning the substring of filename one past '/'. Since
    // library filenames include the namespace in which the file occurs, short filenames are not ambiguous.

  static struct timespec epoch();
    // Return the 'epoch' attribute value

  static void resetEpoch();
    // Assign to 'epoch' attribute the value returned by 'clock_gettime' with clock type 'CLOCK_TYPE'.
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
#define REINVENT_UTIL_LOG_FATAL_VARGS(callerFormat, ...)                                                          \
  if (true) {                                                                                                     \
    unsigned sec, ns;                                                                                             \
    Reinvent::Util::LogRuntime::elapsedTime(&sec, &ns);                                                           \
    fprintf(stderr, REINVENT_UTIL_LOG_CONCAT("%06u.%09u %s %s:%d ", callerFormat), sec, ns,                       \
      Reinvent::Util::LogRuntime::FATAL_TAG, Reinvent::Util::LogRuntime::filename(__FILE__),                      \
      __LINE__ REINVENT_UTIL_LOG_VA_ARGS(__VA_ARGS__));                                                           \
    fflush(stderr);                                                                                               \
  }

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_ERROR
#define REINVENT_UTIL_LOG_ERROR_VARGS(callerFormat, ...)                                                          \
  if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_ERROR,1)) {    \
    unsigned sec, ns;                                                                                             \
    Reinvent::Util::LogRuntime::elapsedTime(&sec, &ns);                                                           \
    fprintf(stderr, REINVENT_UTIL_LOG_CONCAT("%06u.%09u %s %s:%d ", callerFormat), sec, ns,                       \
      Reinvent::Util::LogRuntime::ERROR_TAG, Reinvent::Util::LogRuntime::filename(__FILE__),                      \
      __LINE__ REINVENT_UTIL_LOG_VA_ARGS(__VA_ARGS__));                                                           \
    fflush(stderr);                                                                                               \
  }
#else
#define REINVENT_UTIL_LOG_ERROR_VARGS(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_WARN
#define REINVENT_UTIL_LOG_WARN_VARGS(callerFormat, ...)                                                           \
  if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_WARN,1)) {     \
    unsigned sec, ns;                                                                                             \
    Reinvent::Util::LogRuntime::elapsedTime(&sec, &ns);                                                           \
    fprintf(stderr, REINVENT_UTIL_LOG_CONCAT("%06u.%09u %s %s:%d ", callerFormat), sec, ns,                       \
      Reinvent::Util::LogRuntime::WARN_TAG, Reinvent::Util::LogRuntime::filename(__FILE__),                       \
      __LINE__ REINVENT_UTIL_LOG_VA_ARGS(__VA_ARGS__));                                                           \
  }
#else
#define REINVENT_UTIL_LOG_WARN_VARGS(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_INFO
#define REINVENT_UTIL_LOG_INFO_VARGS(callerFormat, ...)                                                           \
  if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_INFO,1)) {     \
    unsigned sec, ns;                                                                                             \
    Reinvent::Util::LogRuntime::elapsedTime(&sec, &ns);                                                           \
    fprintf(stderr, REINVENT_UTIL_LOG_CONCAT("%06u.%09u %s %s:%d ", callerFormat), sec, ns,                       \
      Reinvent::Util::LogRuntime::INFO_TAG, Reinvent::Util::LogRuntime::filename(__FILE__),                       \
      __LINE__ REINVENT_UTIL_LOG_VA_ARGS(__VA_ARGS__));                                                           \
  }
#else
#define REINVENT_UTIL_LOG_INFO_VARGS(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_DEBUG
#define REINVENT_UTIL_LOG_DEBUG_VARGS(callerFormat, ...)                                                          \
  if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_DEBUG,1)) {    \
    unsigned sec, ns;                                                                                             \
    Reinvent::Util::LogRuntime::elapsedTime(&sec, &ns);                                                           \
    fprintf(stderr, REINVENT_UTIL_LOG_CONCAT("%06u.%09u %s %s:%d ", callerFormat), sec, ns,                       \
      Reinvent::Util::LogRuntime::DEBUG_TAG, Reinvent::Util::LogRuntime::filename(__FILE__),                      \
      __LINE__ REINVENT_UTIL_LOG_VA_ARGS(__VA_ARGS__));                                                           \
  }
#else
#define REINVENT_UTIL_LOG_DEBUG_VARGS(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_TRACE
#define REINVENT_UTIL_LOG_TRACE_VARGS(callerFormat, ...)                                                          \
  if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_TRACE,1)) {    \
    unsigned sec, ns;                                                                                             \
    Reinvent::Util::LogRuntime::elapsedTime(&sec, &ns);                                                           \
    fprintf(stderr, REINVENT_UTIL_LOG_CONCAT("%06u.%09u %s %s:%d ", callerFormat), sec, ns,                       \
      Reinvent::Util::LogRuntime::TRACE_TAG, Reinvent::Util::LogRuntime::filename(__FILE__),                      \
      __LINE__ REINVENT_UTIL_LOG_VA_ARGS(__VA_ARGS__));                                                           \
  }
#else
#define REINVENT_UTIL_LOG_TRACE_VARGS(...) ((void)0)
#endif

//
// Stream style logging macros. Fatal is always enabled. Note that caller always provides a terminating ';' which 
// follows the macro invocation. Also note a new line terminator is provided only if caller's stream includes it. The
// macros do not add line delimiters.
//
#define REINVENT_UTIL_LOG_FATAL(REINVENT_UTIL_LOG_STREAM_ARGS)                                                      \
  if (true) {                                                                                                       \
    char log[Reinvent::Util::DEFAULT_FORMAT_SIZE];                                                                  \
    Reinvent::Util::LogRuntime::logTimestampAndSeverity(log, Reinvent::Util::LogRuntime::FATAL_TAG);                \
    std::lock_guard<std::mutex> grd(Reinvent::Util::LogRuntime::lock);                                              \
    std::cerr << log                                                                                                \
    << Reinvent::Util::LogRuntime::filename(__FILE__)                                                               \
    << ":"                                                                                                          \
    << __LINE__                                                                                                     \
    << " "                                                                                                          \
    << REINVENT_UTIL_LOG_STREAM_ARGS                                                                                \
    << std::flush;                                                                                                  \
  }

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_ERROR
#define REINVENT_UTIL_LOG_ERROR(REINVENT_UTIL_LOG_STREAM_ARGS)                                                      \
  if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_ERROR,1)) {      \
    char log[Reinvent::Util::DEFAULT_FORMAT_SIZE];                                                                  \
    Reinvent::Util::LogRuntime::logTimestampAndSeverity(log, Reinvent::Util::LogRuntime::ERROR_TAG);                \
    std::lock_guard<std::mutex> grd(Reinvent::Util::LogRuntime::lock);                                              \
    std::cerr << log                                                                                                \
    << Reinvent::Util::LogRuntime::filename(__FILE__)                                                               \
    << ":"                                                                                                          \
    << __LINE__                                                                                                     \
    << " "                                                                                                          \
    << REINVENT_UTIL_LOG_STREAM_ARGS                                                                                \
    << std::flush;                                                                                                  \
  }
#else
#define REINVENT_UTIL_LOG_ERROR_(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_WARN
#define REINVENT_UTIL_LOG_WARN(REINVENT_UTIL_LOG_STREAM_ARGS)                                                       \
  if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_WARN,1)) {       \
    char log[Reinvent::Util::DEFAULT_FORMAT_SIZE];                                                                  \
    Reinvent::Util::LogRuntime::logTimestampAndSeverity(log, Reinvent::Util::LogRuntime::WARN_TAG);                 \
    std::lock_guard<std::mutex> grd(Reinvent::Util::LogRuntime::lock);                                              \
    std::cerr << log                                                                                                \
    << Reinvent::Util::LogRuntime::filename(__FILE__)                                                               \
    << ":"                                                                                                          \
    << __LINE__                                                                                                     \
    << " "                                                                                                          \
    << REINVENT_UTIL_LOG_STREAM_ARGS;                                                                               \
  }
#else
#define REINVENT_UTIL_LOG_WARN(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_INFO
#define REINVENT_UTIL_LOG_INFO(REINVENT_UTIL_LOG_STREAM_ARGS)                                                       \
  if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_INFO,1)) {       \
    char log[Reinvent::Util::DEFAULT_FORMAT_SIZE];                                                                  \
    Reinvent::Util::LogRuntime::logTimestampAndSeverity(log, Reinvent::Util::LogRuntime::INFO_TAG);                 \
    std::lock_guard<std::mutex> grd(Reinvent::Util::LogRuntime::lock);                                              \
    std::cerr << log                                                                                                \
    << Reinvent::Util::LogRuntime::filename(__FILE__)                                                               \
    << ":"                                                                                                          \
    << __LINE__                                                                                                     \
    << " "                                                                                                          \
    << REINVENT_UTIL_LOG_STREAM_ARGS;                                                                               \
  }
#else
#define REINVENT_UTIL_LOG_INFO(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_DEBUG
#define REINVENT_UTIL_LOG_DEBUG(REINVENT_UTIL_LOG_STREAM_ARGS)                                                      \
  if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_DEBUG,1)) {      \
    char log[Reinvent::Util::DEFAULT_FORMAT_SIZE];                                                                  \
    Reinvent::Util::LogRuntime::logTimestampAndSeverity(log, Reinvent::Util::LogRuntime::DEBUG_TAG);                \
    std::lock_guard<std::mutex> grd(Reinvent::Util::LogRuntime::lock);                                              \
    std::cerr << log                                                                                                \
    << Reinvent::Util::LogRuntime::filename(__FILE__)                                                               \
    << ":"                                                                                                          \
    << __LINE__                                                                                                     \
    << " "                                                                                                          \
    << REINVENT_UTIL_LOG_STREAM_ARGS;                                                                               \
  }
#else
#define REINVENT_UTIL_LOG_DEBUG(...) ((void)0)
#endif

#if REINVENT_UTIL_LOGGING_BUILD_SEVERITY >= REINVENT_UTIL_LOGGING_SEVERITY_TRACE
#define REINVENT_UTIL_LOG_TRACE(REINVENT_UTIL_LOG_STREAM_ARGS)                                                      \
  if (__builtin_expect(Reinvent::Util::LogRuntime::severityLimit()>=REINVENT_UTIL_LOGGING_SEVERITY_TRACE,1)) {      \
    char log[Reinvent::Util::DEFAULT_FORMAT_SIZE];                                                                  \
    Reinvent::Util::LogRuntime::logTimestampAndSeverity(log, Reinvent::Util::LogRuntime::TRACE_TAG);                \
    std::cerr << log                                                                                                \
    << Reinvent::Util::LogRuntime::filename(__FILE__)                                                               \
    << ":"                                                                                                          \
    << __LINE__                                                                                                     \
    << " "                                                                                                          \
    << REINVENT_UTIL_LOG_STREAM_ARGS;                                                                               \
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
