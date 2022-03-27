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

  ../../src/reinvent/perf/reinvent_perf_rdpmc.cpp

  test1.cpp
) 

#
# Build a test to micro-benchmark section 1
#
set(OPT_TEST_TARGET_SECTION1 reinvent_perf_opt_sec1_test1.tsk)
add_executable(${OPT_TEST_TARGET_SECTION1} ${SOURCES})
target_compile_options(${OPT_TEST_TARGET_SECTION1} PUBLIC -g -O2)
target_compile_options(${OPT_TEST_TARGET_SECTION1} PUBLIC -Wno-unused-variable)                                               
target_compile_options(${OPT_TEST_TARGET_SECTION1} PUBLIC -Wno-unused-parameter)
target_include_directories(${OPT_TEST_TARGET_SECTION1} PUBLIC ../../src/reinvent)                                                   
target_compile_definitions(${OPT_TEST_TARGET_SECTION1} PUBLIC NDEBUG)
target_compile_definitions(${OPT_TEST_TARGET_SECTION1} PUBLIC PERF_SECTION1)
target_link_libraries(${OPT_TEST_TARGET_SECTION1} PUBLIC ${DPDK_SELECTED_LIBRARIES})

#
# Build a test to micro-benchmark section 2
#
set(OPT_TEST_TARGET_SECTION2 reinvent_perf_opt_sec2_test1.tsk)
add_executable(${OPT_TEST_TARGET_SECTION2} ${SOURCES})
target_compile_options(${OPT_TEST_TARGET_SECTION2} PUBLIC -O2)
target_compile_options(${OPT_TEST_TARGET_SECTION2} PUBLIC -Wno-unused-variable)                                               
target_compile_options(${OPT_TEST_TARGET_SECTION2} PUBLIC -Wno-unused-parameter)
target_include_directories(${OPT_TEST_TARGET_SECTION2} PUBLIC ../../src/reinvent)                                                   
target_compile_definitions(${OPT_TEST_TARGET_SECTION2} PUBLIC NDEBUG)
target_compile_definitions(${OPT_TEST_TARGET_SECTION2} PUBLIC PERF_SECTION2)
target_link_libraries(${OPT_TEST_TARGET_SECTION2} PUBLIC ${DPDK_SELECTED_LIBRARIES})

#
# Build a test to micro-benchmark section 3
#
set(OPT_TEST_TARGET_SECTION3 reinvent_perf_opt_sec3_test1.tsk)
add_executable(${OPT_TEST_TARGET_SECTION3} ${SOURCES})
target_compile_options(${OPT_TEST_TARGET_SECTION3} PUBLIC -O2)
target_compile_options(${OPT_TEST_TARGET_SECTION3} PUBLIC -Wno-unused-variable)                                               
target_compile_options(${OPT_TEST_TARGET_SECTION3} PUBLIC -Wno-unused-parameter)
target_include_directories(${OPT_TEST_TARGET_SECTION3} PUBLIC ../../src/reinvent)                                                   
target_compile_definitions(${OPT_TEST_TARGET_SECTION3} PUBLIC NDEBUG)
target_compile_definitions(${OPT_TEST_TARGET_SECTION3} PUBLIC PERF_SECTION3)
target_link_libraries(${OPT_TEST_TARGET_SECTION3} PUBLIC ${DPDK_SELECTED_LIBRARIES})

#
# Build a test to micro-benchmark entire client loop
#
set(OPT_TEST_TARGET_SECTION_ALL reinvent_perf_opt_all_test1.tsk)
add_executable(${OPT_TEST_TARGET_SECTION_ALL} ${SOURCES})
target_compile_options(${OPT_TEST_TARGET_SECTION_ALL} PUBLIC -O2)
target_compile_options(${OPT_TEST_TARGET_SECTION_ALL} PUBLIC -Wno-unused-variable)                                               
target_compile_options(${OPT_TEST_TARGET_SECTION_ALL} PUBLIC -Wno-unused-parameter)
target_include_directories(${OPT_TEST_TARGET_SECTION_ALL} PUBLIC ../../src/reinvent)                                                   
target_compile_definitions(${OPT_TEST_TARGET_SECTION_ALL} PUBLIC NDEBUG)
target_compile_definitions(${OPT_TEST_TARGET_SECTION_ALL} PUBLIC PERF_ALL)
target_link_libraries(${OPT_TEST_TARGET_SECTION_ALL} PUBLIC ${DPDK_SELECTED_LIBRARIES})
