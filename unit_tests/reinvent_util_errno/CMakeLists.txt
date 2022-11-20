cmake_minimum_required(VERSION 3.16)

#
# To avoid conflicts with how the reinvent library is built which
# may or may not have logging turned on, these tests bring in all
# library code required. Build is not dependent on libreinvent.a
#

set(TEST_TARGET reinvent_util_errno_unit_test.tsk)

set(SOURCES                                                                                                             
  ../../src/reinvent/util/reinvent_util_log.cpp
  ../../src/reinvent/util/reinvent_util_errno.cpp

  reinvent_util_errno_unit_test.cpp
) 

add_executable(${TEST_TARGET} ${SOURCES})

target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_ON)                                              
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_BUILD_SEVERITY=REINVENT_UTIL_LOGGING_SEVERITY_TRACE)
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_ERRNO)                                           
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_SHORT_FILENAME)                                           
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_TEST_FORMAT)                                           
target_compile_definitions(${TEST_TARGET} PUBLIC NDEBUG)                                                                    
target_include_directories(${TEST_TARGET} PUBLIC ../../src/reinvent)

target_link_libraries(${TEST_TARGET} PUBLIC gtest pthread)

add_test(
    NAME    ${TEST_TARGET}
    COMMAND "<TARGET_FILE:${TEST_TARGET}>"
)
