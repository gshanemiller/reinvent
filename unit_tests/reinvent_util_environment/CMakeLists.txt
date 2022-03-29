cmake_minimum_required(VERSION 3.16)

#
# To avoid conflicts with how the reinvent library is built which
# may or may not have logging turned on, these tests bring in all
# library code required. Build is not dependent on libreinvent.a
#

set(TEST_TARGET reinvent_util_environment_unit_test.tsk)

set(SOURCES                                                                                                             
  ../../src/reinvent/util/reinvent_util_log.cpp
  ../../src/reinvent/util/reinvent_util_errno.cpp
  ../../src/reinvent/util/reinvent_util_environment.cpp

  reinvent_util_environment_unit_test.cpp

  reinvent_util_environment_unit_test_int.cpp
  reinvent_util_environment_unit_test_int_data.cpp

  reinvent_util_environment_unit_test_intlist.cpp
  reinvent_util_environment_unit_test_intlist_data.cpp

  reinvent_util_environment_unit_test_str.cpp
  reinvent_util_environment_unit_test_str_data.cpp
) 

add_executable(${TEST_TARGET} ${SOURCES})

target_compile_options(${TEST_TARGET} PUBLIC -g -O0 --coverage -fprofile-arcs -ftest-coverage)
target_compile_definitions(${TEST_TARGET} PUBLIC NDEBUG)
target_link_options(${TEST_TARGET} PUBLIC --coverage -fprofile-arcs -ftest-coverage)

target_include_directories(${TEST_TARGET} PUBLIC . ../../src/reinvent)

target_link_libraries(${TEST_TARGET} PUBLIC gtest pthread)

add_test(
    NAME    ${TEST_TARGET}
    COMMAND "<TARGET_FILE:${TEST_TARGET}>"
)
