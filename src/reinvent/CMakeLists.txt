cmake_minimum_required(VERSION 3.16)

set(SOURCES
  util/reinvent_util_log.cpp
  util/reinvent_util_environment.cpp
  util/reinvent_util_errno.cpp
 
  dpdk/reinvent_dpdk_names.cpp
  dpdk/reinvent_dpdk_lcore.cpp
  dpdk/reinvent_dpdk_vcpu.cpp
  dpdk/reinvent_dpdk_rxq.cpp
  dpdk/reinvent_dpdk_txq.cpp
  dpdk/reinvent_dpdk_ipv4route.cpp
  dpdk/reinvent_dpdk_config.cpp
  dpdk/reinvent_dpdk_init.cpp
  dpdk/reinvent_dpdk_worker.cpp
  dpdk/reinvent_dpdk_stream.cpp
  dpdk/reinvent_dpdk_util.cpp
)

#
# Optimized library -O2, no aserts, no errno logging
# and only logs WARN or less in code
#
add_library(reinvent_opt ${SOURCES})
target_compile_options(reinvent_opt PUBLIC -O2)
target_compile_definitions(reinvent_opt PUBLIC REINVENT_UTIL_LOGGING_ON)
target_compile_definitions(reinvent_opt PUBLIC REINVENT_UTIL_LOGGING_BUILD_SEVERITY=REINVENT_UTIL_LOGGING_SEVERITY_WARN)
target_compile_definitions(reinvent_opt PUBLIC NDEBUG)
target_include_directories(reinvent_opt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#
# Debug library -g, asserts, errno logging, and logs up to TRACE in code
#
add_library(reinvent_dbg ${SOURCES})
target_compile_options(reinvent_dbg PUBLIC -g)
target_compile_definitions(reinvent_dbg PUBLIC REINVENT_UTIL_LOGGING_ON)
target_compile_definitions(reinvent_dbg PUBLIC REINVENT_UTIL_LOGGING_BUILD_SEVERITY=REINVENT_UTIL_LOGGING_SEVERITY_TRACE)
target_compile_definitions(reinvent_dbg PUBLIC REINVENT_UTIL_LOGGING_ERRNO)
target_include_directories(reinvent_dbg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
