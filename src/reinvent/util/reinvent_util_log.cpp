#include <util/reinvent_util_log.h>

#include <assert.h>
#include <string.h>

namespace Reinvent {

//
// Default runtime log severity is WARN
//
unsigned Util::LogRuntime::d_severityLimit=2;

//
// Default epoch time is 0sec 0ns
//
struct timespec Util::LogRuntime::d_epoch = {0,0};

//
// Default lables, datetime format
//
const char *Util::LogRuntime::ERRNO_TAG = "ERRNO";
const char *Util::LogRuntime::FATAL_TAG = "FATAL";
const char *Util::LogRuntime::ERROR_TAG = "ERROR";
const char *Util::LogRuntime::WARN_TAG  = "WARN ";
const char *Util::LogRuntime::INFO_TAG  = "INFO ";
const char *Util::LogRuntime::DEBUG_TAG = "DEBUG";
const char *Util::LogRuntime::TRACE_TAG = "TRACE";

const char *Util::LogRuntime::FORMAT = "%06u.%09u %s ";

const clockid_t Util::LogRuntime::CLOCK_TYPE = CLOCK_REALTIME;

std::mutex Util::LogRuntime::lock;

unsigned Util::LogRuntime::severityLimit() {
  return Util::LogRuntime::d_severityLimit;
}

int Util::LogRuntime::setSeverityLimit(unsigned level) {
  if (level>0&&level<=REINVENT_UTIL_LOGGING_BUILD_SEVERITY) {
    d_severityLimit = level;
    return 0;
  }
  return 1;
}

void Util::LogRuntime::elapsedTime(unsigned *sec, unsigned *ns) {
  assert(sec);
  assert(ns);

#ifndef REINVENT_UTIL_LOGGING_TEST_FORMAT
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);

  if (now.tv_nsec<d_epoch.tv_nsec) {
    *ns = now.tv_nsec - d_epoch.tv_nsec + 1000000000;
    *sec = --now.tv_sec - d_epoch.tv_sec;
  } else {
    *ns = now.tv_nsec - d_epoch.tv_nsec;
    *sec = now.tv_sec - d_epoch.tv_sec;
  }
# else
  *ns = *sec = 0;
#endif
}

void Util::LogRuntime::logTimestampAndSeverity(char *buf, const char *severity) {
  assert(severity!=0);

  unsigned sec(0);
  unsigned ns(0);
  elapsedTime(&sec, &ns);

  snprintf(buf, Util::DEFAULT_FORMAT_SIZE, Util::LogRuntime::FORMAT, sec, ns, severity);
}

const char *Util::LogRuntime::filename(const char *filename) {
#ifdef REINVENT_UTIL_LOGGING_SHORT_FILENAME
  const char *ch = strrchr(filename, '/');
  return (ch) ? (ch+1) : filename; 
#else
  return filename;
#endif
}

struct timespec Util::LogRuntime::epoch() {
  return d_epoch;
}

void Util::LogRuntime::resetEpoch() {
  clock_gettime(Util::LogRuntime::CLOCK_TYPE, &d_epoch);
}

} // Reinvent
