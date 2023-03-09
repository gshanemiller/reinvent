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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/root/local/include")

#
# DPDK Link Libraries
#
# Requires pre-exported ENV, for example, as done in ./install:
#   PKG_CONFIG_PATH=<path-to-DPDK-pkgconfig>
#
if (DPDK_STATIC_LINK)
  execute_process(
    COMMAND /usr/bin/pkg-config --static --libs libdpdk
    RESULT_VARIABLE DPDK_LIB_RESULT
    OUTPUT_VARIABLE DPDK_SELECTED_LIBRARIES
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  message(STATUS "Reinvent will static link to DPDK: ${DPDK_SELECTED_LIBRARIES}")
else()
  execute_process(
    COMMAND /usr/bin/pkg-config --libs libdpdk
    RESULT_VARIABLE DPDK_LIB_RESULT
    OUTPUT_VARIABLE DPDK_SELECTED_LIBRARIES
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  message(STATUS "Reinvent will dynamic link to DPDK: ${DPDK_SELECTED_LIBRARIES}")
endif()

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
