#include <util/reinvent_util_errno.h>

#include <unistd.h>
#include <string.h>

void parseCommandLine(int argc, char **argv) {
  int c;

  while ((c = getopt (argc, argv, "abc:")) != -1) {
    switch(c) {
      default:
        break;
    }
  }
}

int reinvent_util_errno_notfound() {
  const bool valid(false);

  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND, valid, "objectName");
  }

  return 0;
}

int reinvent_util_errno_atoi() {
  bool valid(false);
  const std::string value("-123ABC");

  valid = value.empty();

  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ATOI, valid, value.c_str());
  }

  return 0;
}

int reinvent_util_errno_atob() {
  bool valid(false);
  const std::string value("notOK");

  valid = value.empty();

  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ATOI, valid, value.c_str());
  }

  return 0;
}

int reinvent_util_errno_range() {
  bool valid(false);
  const std::string variable("objectName");
  const std::string value("1000");
  const int min(0);
  const int max(5);
  const int bad(1000);

  valid = bad>=min && bad<=max;

  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_RANGE, valid, variable.c_str(),
      "environment value", bad, min, max);
  }

  return 0;
}

int reinvent_util_errno_range_list() {
  bool valid(false);
  const std::string variable("objectName");
  const std::string value("[1,2,3,5,8,1000,13,21]");
  const unsigned min(0);
  const unsigned max(5);
  const int bad(1000);

  valid = bad>=min && bad<=max;

  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_RANGE_LIST, valid, variable.c_str(),
      value.c_str(), "contains an element", bad, min, max);
  }

  return 0;
}

int reinvent_util_errno_string_length() {
  bool valid(false);
  const std::string variable("objectName");
  const std::string value("euiru3uidjxjdjwiuwiu22");
  const unsigned min(1);
  const unsigned max(5);

  valid = value.size()>=static_cast<unsigned>(min) && value.size()<=static_cast<unsigned>(max);

  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_STRING_LENGTH, valid, variable.c_str(),
      value.c_str(), "contains string element with length", value.size(), min, max);
  }

  return 0;
}

int reinvent_util_errno_environment() {
  bool valid(false);
  const std::string variable("objectName");
  const std::string value("0x1235");

  valid = value.empty(); 
  
  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, valid, variable.c_str(),
      value.c_str(), "is not an even number");
  }

  return 0;
}

int reinvent_util_errno_no_resource() {
  bool valid(false);
  const std::string variable("resourceName");
  const int value(10);
  const int available(5);

  valid = value<available;

  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, valid, variable.c_str(), value,
      available, "are availble for use");
  }

  return 0;
}

int reinvent_util_errno_api() {
  const int rc(2);
  bool valid(false);

  valid = rc==0;

  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_API, valid, "cannot open file",
      strerror(rc), rc);
  }

  return 0;
}

int main(int argc, char **argv) {
  parseCommandLine(argc, argv);

  Reinvent::Util::LogRuntime::resetEpoch();
  Reinvent::Util::LogRuntime::setSeverityLimit(REINVENT_UTIL_LOGGING_SEVERITY_TRACE);

  //
  // For each library errno type run a function fails
  //
  int rc = reinvent_util_errno_notfound();
  REINVENT_UTIL_LOG_INFO_VARGS("reinvent_util_errno_notfound rc=%d\n", rc);

  rc = reinvent_util_errno_atoi();
  REINVENT_UTIL_LOG_INFO_VARGS("reinvent_util_errno_atoi rc=%d\n", rc);

  rc = reinvent_util_errno_atob();
  REINVENT_UTIL_LOG_INFO_VARGS("reinvent_util_errno_atob rc=%d\n", rc);

  rc = reinvent_util_errno_range();
  REINVENT_UTIL_LOG_INFO_VARGS("reinvent_util_errno_range rc=%d\n", rc);

  rc = reinvent_util_errno_range_list();
  REINVENT_UTIL_LOG_INFO_VARGS("reinvent_util_errno_range_list rc=%d\n", rc);

  rc = reinvent_util_errno_string_length();
  REINVENT_UTIL_LOG_INFO_VARGS("reinvent_util_errno_string_length rc=%d\n", rc);

  rc = reinvent_util_errno_environment();
  REINVENT_UTIL_LOG_INFO_VARGS("reinvent_util_errno_environment rc=%d\n", rc);
  
  rc = reinvent_util_errno_no_resource();
  REINVENT_UTIL_LOG_INFO_VARGS("reinvent_util_errno_no_resource rc=%d\n", rc);

  rc = reinvent_util_errno_api();
  REINVENT_UTIL_LOG_INFO_VARGS("reinvent_util_errno_api rc=%d\n", rc);

  return rc;
}
