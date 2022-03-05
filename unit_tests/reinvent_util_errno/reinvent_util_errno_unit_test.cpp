#include <gtest/gtest.h>

#include <util/reinvent_util_errno.h>

#include <errno.h>
#include <string.h>

using namespace Reinvent;

int test_notfound() {
   REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND, false, "UNIT_TEST_VAR");
}

int test_atoi() {
   REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ATOI, false, "--123");
}

int test_atob() {
   REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ATOB, false, "not a truth value");
}

int test_range() {
   REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_RANGE, false, "UNIT_TEST_VAR", "value", 100, 0, 10);
}

int test_rangelist() {
   REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_RANGE_LIST, false, "UNIT_TEST_VAR", "(1,100,7)", "with value", 100, 1, 5);
}

int test_stringlength() {
   REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_STRING_LENGTH, false, "UNIT_TEST_VAR", "abc123", "with length", 6, 0, 3);
}

int test_environment() {
   REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, false, "UNIT_TEST_VAR", "abc123", "not one of yes, no, even");
}

int test_noresource() {
   REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, false, "memory (kb) of", 1024, 512, "is available");
}

int test_apierror() {
   REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_API, false, "opening file", strerror(2), 2);
}

TEST(util_errno, basic) {  
  EXPECT_EQ(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND,     test_notfound());
  EXPECT_EQ(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ATOI,         test_atoi());
  EXPECT_EQ(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ATOB,         test_atob());
  EXPECT_EQ(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_RANGE,        test_range());
  EXPECT_EQ(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_RANGE_LIST,   test_rangelist());
  EXPECT_EQ(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_STRING_LENGTH,test_stringlength());
  EXPECT_EQ(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT,  test_environment());
  EXPECT_EQ(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE,  test_noresource());
  EXPECT_EQ(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_API,          test_apierror());
}

int main(int argc, char **argv) {                                                                                       
  testing::InitGoogleTest(&argc, argv);                                                                                 
  return RUN_ALL_TESTS();                                                                                               
}
