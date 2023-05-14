#include <util/reinvent_util_log.h>

#include <thread>
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

void logTest(int threadId) {
  // Declare printf data to log
  const int aInt(10);
  const char *aStr="printf I/O log";
  const void * ptr = static_cast<const void*>(&aInt);

  // Declare stream data to log
  const float bFloat(1.78);
  const char *bStr="stream based I/O log ";
  std::string bInfo(" (errno: 10)");

  for (int i=0; i<100; ++i) {
    // Printf based macros 
    REINVENT_UTIL_LOG_FATAL_VARGS("threadId %d charPtr='%s' pointer=%p (rc=%d)\n", threadId, aStr, ptr, aInt);
    REINVENT_UTIL_LOG_ERROR_VARGS("threadId %d charPtr='%s' pointer=%p (rc=%d)\n", threadId, aStr, ptr, aInt);
    REINVENT_UTIL_LOG_WARN_VARGS("threadId %d charPtr='%s' pointer=%p (rc=%d)\n", threadId, aStr, ptr, aInt);
    REINVENT_UTIL_LOG_INFO_VARGS("threadId %d charPtr='%s' pointer=%p (rc=%d)\n", threadId, aStr, ptr, aInt);
    REINVENT_UTIL_LOG_DEBUG_VARGS("threadId %d charPtr='%s' pointer=%p (rc=%d)\n", threadId, aStr, ptr, aInt);
    REINVENT_UTIL_LOG_TRACE_VARGS("threadId %d charPtr='%s' pointer=%p (rc=%d)\n", threadId, aStr, ptr, aInt);

    // Stream based macros
    REINVENT_UTIL_LOG_FATAL("threadId:" << threadId << " " <<bStr << bFloat << bInfo << std::endl);
    REINVENT_UTIL_LOG_ERROR("threadId:" << threadId << " " <<bStr << bFloat << bInfo << std::endl);
    REINVENT_UTIL_LOG_WARN("threadId:" << threadId << " " <<bStr << bFloat << bInfo << std::endl);
    REINVENT_UTIL_LOG_INFO("threadId:" << threadId << " " <<bStr << bFloat << bInfo << std::endl);
    REINVENT_UTIL_LOG_DEBUG("threadId:" << threadId << " " <<bStr << bFloat << bInfo << std::endl);
    REINVENT_UTIL_LOG_TRACE("threadId:" << threadId << " " <<bStr << bFloat << bInfo << std::endl);
  }
}

int main(int argc, char **argv) {
  parseCommandLine(argc, argv);

  Reinvent::Util::LogRuntime::resetEpoch();
  Reinvent::Util::LogRuntime::setSeverityLimit(REINVENT_UTIL_LOGGING_SEVERITY_TRACE);

  std::thread t1(logTest, 1);
  std::thread t2(logTest, 2);
  std::thread t3(logTest, 3);
  std::thread t4(logTest, 4);
  std::thread t5(logTest, 5);
  
  t1.join(); 
  t2.join(); 
  t3.join(); 
  t4.join(); 
  t5.join(); 
 
  return 0;
}
