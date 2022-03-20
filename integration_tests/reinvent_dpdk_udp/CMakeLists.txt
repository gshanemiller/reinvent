cmake_minimum_required(VERSION 3.16)

#
# To avoid conflicts with how the reinvent library is built which
# may or may not have logging turned on, these tests bring in all
# library code required. Build is not dependent on libreinvent.a
#

set(SOURCES                                                                                                             
  ../../src/reinvent/dpdk/reinvent_dpdk_awsconfig.cpp
  ../../src/reinvent/dpdk/reinvent_dpdk_awsworker.cpp
  ../../src/reinvent/dpdk/reinvent_dpdk_initaws.cpp
  ../../src/reinvent/dpdk/reinvent_dpdk_lcore.cpp
  ../../src/reinvent/dpdk/reinvent_dpdk_names.cpp
  ../../src/reinvent/dpdk/reinvent_dpdk_rxq.cpp
  ../../src/reinvent/dpdk/reinvent_dpdk_udproute.cpp
  ../../src/reinvent/dpdk/reinvent_dpdk_txq.cpp
  ../../src/reinvent/dpdk/reinvent_dpdk_uninitaws.cpp
  ../../src/reinvent/dpdk/reinvent_dpdk_vcpu.cpp
  ../../src/reinvent/dpdk/reinvent_dpdk_stream.cpp
  ../../src/reinvent/dpdk/reinvent_dpdk_util.cpp

  ../../src/reinvent/util/reinvent_util_log.cpp
  ../../src/reinvent/util/reinvent_util_errno.cpp
  ../../src/reinvent/util/reinvent_util_environment.cpp

  reinvent_dpdk_udp_integration_test.cpp
  libperf.c
) 

#
# Build a test with logging ON for all severities at build time
#
set(TEST_TARGET reinvent_dpdk_udp_integration_test.tsk)
add_executable(${TEST_TARGET} ${SOURCES})
target_compile_options(${TEST_TARGET} PUBLIC -g)
target_include_directories(${TEST_TARGET} PUBLIC ../../src/reinvent)                                                   
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_ON)
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_BUILD_SEVERITY=REINVENT_UTIL_LOGGING_SEVERITY_TRACE)
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_ERRNO)
target_compile_definitions(${TEST_TARGET} PUBLIC REINVENT_UTIL_LOGGING_SHORT_FILENAME)
target_compile_definitions(${TEST_TARGET} PUBLIC NDEBUG)
target_link_libraries(${TEST_TARGET} PUBLIC ${DPDK_SELECTED_LIBRARIES})

#
# Build a test with logging off optimized
#
set(OPT_TEST_TARGET reinvent_dpdk_udp_opt_integration_test.tsk)
add_executable(${OPT_TEST_TARGET} ${SOURCES})
target_compile_options(${OPT_TEST_TARGET} PUBLIC -O2)
target_compile_options(${OPT_TEST_TARGET} PUBLIC -Wno-unused-variable)                                               
target_compile_options(${OPT_TEST_TARGET} PUBLIC -Wno-unused-parameter)
target_include_directories(${OPT_TEST_TARGET} PUBLIC ../../src/reinvent)                                                   
target_compile_definitions(${OPT_TEST_TARGET} PUBLIC NDEBUG)
target_link_libraries(${OPT_TEST_TARGET} PUBLIC ${DPDK_SELECTED_LIBRARIES})
