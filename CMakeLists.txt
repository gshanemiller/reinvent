cmake_minimum_required(VERSION 3.16)
project(reinvent)

# Define location of build artifacts
#
set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_PDB_OUTPUT_DIRECTORY         ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY     ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY     ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY     ${CMAKE_SOURCE_DIR}/build)

#
# Set project wide compilation options
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-diagnostics-color")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/home/ec2-user/local/include")

#
# DPDK Link Libraries
#
#
# Approach 1: Link all DPDK 170+ libraries
#
# Requires pre-exported ENV:
#   PKG_CONFIG_PATH=/home/ec2-user/local/lib64/pkgconfig
#
# execute_process(
#   COMMAND /usr/bin/pkg-config --static --libs libdpdk
#   RESULT_VARIABLE DPDK_LIB_RESULT
#   OUTPUT_VARIABLE DPDK_SELECTED_LIBRARIES
#   OUTPUT_STRIP_TRAILING_WHITESPACE
# )
# message(STATUS "pkg-config DPDK: ${DPDK_SELECTED_LIBRARIES}")

#
# Approach 2: (Default) Link selected DPDK libraries only
#
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} -L/home/ec2-user/local/lib64)
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} -Wl,--whole-archive)
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} -lrte_ethdev)
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} -lrte_telemetry)
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} -lrte_kvargs)
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} -lrte_mbuf)
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} -lrte_mempool)
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} -lrte_timer)
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} -lrte_eal)
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} -lrte_net_ena)
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} -lrte_net)
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} -Wl,--no-whole-archive)
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} dl)
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} numa)
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} rt)
set(DPDK_SELECTED_LIBRARIES ${DPDK_SELECTED_LIBRARIES} pthread)
message(STATUS "DPDK selected libraries: ${DPDK_SELECTED_LIBRARIES}")

#
# Add library code for building library
#
add_subdirectory(src)

#
# Add unit tests if enabled
#
if(UNIT_TEST)
  message(STATUS "Unit tests enabled")
  enable_testing()
  add_subdirectory(unit_tests)
else()
  message(STATUS "Unit tests disabled")
endif()

#
# Add integration tests if enabled
#
if(INTEGRATION_TEST)
  message(STATUS "Integration tests enabled")
  add_subdirectory(integration_tests)
else()
  message(STATUS "Integration tests disabled")
endif()

#
# Add performance tests if enabled
#
if(PERFORMANCE_TEST)
  message(STATUS "Performance tests enabled")
  add_subdirectory(performance_tests)
else()
  message(STATUS "Performance tests disabled")
endif()
