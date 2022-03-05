#include <util/reinvent_util_log.h>

#include <unistd.h>

void parseCommandLine(int argc, char **argv) {
  int c;

  while ((c = getopt (argc, argv, "abc:")) != -1) {
    switch(c) {
      default:
        break;
    }
  }
}

int main(int argc, char **argv) {
  parseCommandLine(argc, argv);

  Reinvent::Util::LogRuntime::resetEpoch();
  Reinvent::Util::LogRuntime::setSeverityLimit(REINVENT_UTIL_LOGGING_SEVERITY_TRACE);

  // Declare printf data to log
  const int aInt(10);
  const char *aStr="printf I/O log";
  const char *format="charPtr=%s pointer=%x (rc=%d)\n";

  // Printf based macros 
  REINVENT_UTIL_LOG_FATAL_VARGS(format, aStr, &aInt, aInt);
  REINVENT_UTIL_LOG_ERROR_VARGS(format, aStr, &aInt, aInt);
  REINVENT_UTIL_LOG_WARN_VARGS(format, aStr, &aInt, aInt);
  REINVENT_UTIL_LOG_INFO_VARGS(format, aStr, &aInt, aInt);
  REINVENT_UTIL_LOG_DEBUG_VARGS(format, aStr, &aInt, aInt);
  REINVENT_UTIL_LOG_TRACE_VARGS(format, aStr, &aInt, aInt);

  // Declare stream data to log
  const float bFloat(1.78);
  const char *bStr="stream based I/O log ";
  std::string bInfo(" (errno: 10)");

  // Stream based macros
  REINVENT_UTIL_LOG_FATAL(bStr << bFloat << bInfo << std::endl);
  REINVENT_UTIL_LOG_ERROR(bStr << bFloat << bInfo << std::endl);
  REINVENT_UTIL_LOG_WARN(bStr << bFloat << bInfo << std::endl);
  REINVENT_UTIL_LOG_INFO(bStr << bFloat << bInfo << std::endl);
  REINVENT_UTIL_LOG_DEBUG(bStr << bFloat << bInfo << std::endl);
  REINVENT_UTIL_LOG_TRACE(bStr << bFloat << bInfo << std::endl);
 
  return 0;
}
