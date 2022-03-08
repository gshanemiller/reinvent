cmake_minimum_required(VERSION 3.16)

#
# To avoid conflicts with how the reinvent library is built which
# may or may not have logging turned on, these tests bring in all
# library code required. Build is not dependent on libreinvent.a
#

set(SOURCES                                                                                                             
  ../../src/reinvent/util/reinvent_util_log.cpp
  ../../src/reinvent/util/reinvent_util_errno.cpp
  reinvent_util_errno_integration_test.cpp
) 

#
# Build a test with assert on. Task will core if assert true
#
set(TEST_TARGET reinvent_util_errno_integration_test.tsk)
add_executable(${TEST_TARGET} ${SOURCES})
target_compile_options(${TEST_TARGET} PUBLIC -g)
target_include_directories(${TEST_TARGET} PUBLIC ../../src/reinvent)
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_ON)
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_ERRNO)
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_SHORT_FILENAME)                                  
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_TEST_FORMAT)                                     

#
# Build a test with assert off but logging on.
#
set(NO_ASSERT_TEST_TARGET reinvent_util_errno_noassert_integration_test.tsk)
add_executable(${NO_ASSERT_TEST_TARGET} ${SOURCES})
target_include_directories(${NO_ASSERT_TEST_TARGET} PUBLIC ../../src/reinvent)
target_compile_definitions(${NO_ASSERT_TEST_TARGET} PUBLIC NDEBUG)
target_compile_definitions(${NO_ASSERT_TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_ON)
target_compile_definitions(${NO_ASSERT_TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_ERRNO)
target_compile_definitions(${NO_ASSERT_TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_SHORT_FILENAME)                                  
target_compile_definitions(${NO_ASSERT_TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_TEST_FORMAT)                                     

#
# Build a test with assert off, logging off
#
set(NO_ASSERT_NO_LOG_TEST_TARGET reinvent_util_errno_silent_integration_test.tsk.tsk)
add_executable(${NO_ASSERT_NO_LOG_TEST_TARGET} ${SOURCES})
target_compile_options(${NO_ASSERT_NO_LOG_TEST_TARGET} PUBLIC -g -N)
target_compile_definitions(${NO_ASSERT_NO_LOG_TEST_TARGET} PUBLIC NDEBUG)
target_include_directories(${NO_ASSERT_NO_LOG_TEST_TARGET} PUBLIC ../../src/reinvent)
