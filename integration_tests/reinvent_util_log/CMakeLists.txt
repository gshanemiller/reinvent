cmake_minimum_required(VERSION 3.16)

#
# To avoid conflicts with how the reinvent library is built which
# may or may not have logging turned on, these tests bring in all
# library code required. Build is not dependent on libreinvent.a
#

set(SOURCES                                                                                                             
  ../../src/reinvent/util/reinvent_util_log.cpp
  main.cpp
) 

set(SOURCES1                                                                                                             
  ../../src/reinvent/util/reinvent_util_log.cpp
  main_mt.cpp
) 

#
# Build a test with logging ON for all severities at build time
#
set(TEST_TARGET reinvent_util_log_integration_test.tsk)
add_executable(${TEST_TARGET} ${SOURCES})
target_compile_options(${TEST_TARGET} PUBLIC -g)
target_include_directories(${TEST_TARGET} PUBLIC ../../src/reinvent)
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_ON)
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_BUILD_SEVERITY=REINVENT_UTIL_LOGGING_SEVERITY_TRACE)

#
# Define and optimize out all logging (-O2)
#
set(NO_LOG_TEST_TARGET reinvent_util_log_stripped_integration_test.tsk)
add_executable(${NO_LOG_TEST_TARGET} ${SOURCES})
target_compile_options(${NO_LOG_TEST_TARGET} PUBLIC -O2)
target_compile_options(${NO_LOG_TEST_TARGET} PUBLIC -Wno-unused-variable)
target_include_directories(${NO_LOG_TEST_TARGET} PUBLIC ../../src/reinvent)

#
# Build a test with logging ON for all severities at build time but with testing format
#
set(TEST_TARGET_CLEAN_TIMESTAMP reinvent_util_log_cleants_integration_test.tsk)
add_executable(${TEST_TARGET_CLEAN_TIMESTAMP} ${SOURCES})
target_compile_options(${TEST_TARGET_CLEAN_TIMESTAMP} PUBLIC -g)
target_include_directories(${TEST_TARGET_CLEAN_TIMESTAMP} PUBLIC ../../src/reinvent)
target_compile_definitions(${TEST_TARGET_CLEAN_TIMESTAMP} PUBLIC REINVENT_UTIL_LOGGING_ON)
target_compile_definitions(${TEST_TARGET_CLEAN_TIMESTAMP} PUBLIC REINVENT_UTIL_LOGGING_BUILD_SEVERITY=REINVENT_UTIL_LOGGING_SEVERITY_TRACE)
target_compile_definitions(${TEST_TARGET_CLEAN_TIMESTAMP} PUBLIC REINVENT_UTIL_LOGGING_TEST_FORMAT)

#
# Build a MT test with logging ON for all severities at build time
#
set(TEST_TARGET reinvent_util_log_integration_mt_test.tsk)
add_executable(${TEST_TARGET} ${SOURCES1})
target_compile_options(${TEST_TARGET} PUBLIC -g)
target_include_directories(${TEST_TARGET} PUBLIC ../../src/reinvent)
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_ON)
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_BUILD_SEVERITY=REINVENT_UTIL_LOGGING_SEVERITY_TRACE)
target_link_libraries(${TEST_TARGET} PUBLIC pthread)
