cmake_minimum_required(VERSION 2.6)
option(BUILD_FORCE_32 "Set to ON to enable 32 bit compilation" OFF)


IF( ($ENV{CHAOS32}) OR (BUILD_FORCE_32) )
    MESSAGE(STATUS "Enabling 32 bit Compilation")
    set (CMAKE_C_FLAGS "-m32")
    set (CMAKE_CXX_FLAGS "-m32")
    set (CMAKE_LINK_FLAGS "-m32")
ENDIF()

set (CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/../../usr/local)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

INCLUDE_DIRECTORIES(. ${PROJECT_SOURCE_DIR}/../../usr/local/include)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../chaosframework/usr/local/lib)

ADD_DEFINITIONS(-g -O2 -Wall -DBOOST_ALL_DYN_LINK)

SET(cutest_exe_src ControlUnitExample.cpp RTWorkerCU.cpp SCWorkerCU.cpp SinWaveCommand.cpp TestCorrelatingCommand.cpp DummyDriver.cpp)

ADD_EXECUTABLE(ExampleControlUnit ${cutest_exe_src})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ExampleControlUnit pthread chaos_cutoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ExampleControlUnit chaos_cutoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

INSTALL_TARGETS(/bin ExampleControlUnit)
