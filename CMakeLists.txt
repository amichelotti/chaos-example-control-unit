cmake_minimum_required(VERSION 2.8)

project(examplecontrolunit)
include(ExternalProject)

find_package(chaos MODULE REQUIRED)
#include_directories(/Users/bisegni/source/chaos_development/usr/local/include)
#link_directories(/Users/bisegni/source/chaos_development/usr/local/lib)

ExternalProject_Add(
opencv
GIT_REPOSITORY https://github.com/opencv/opencv.git
GIT_TAG master
PREFIX "${CMAKE_BINARY_DIR}/lib/opencv-prefix"
SOURCE_DIR "${CMAKE_BINARY_DIR}/lib/opencv-src"
BINARY_DIR "${CMAKE_BINARY_DIR}/lib/opencv-build"
LOG_DOWNLOAD ON
LOG_CONFIGURE ON
LOG_BUILD ON)

SET(cutest_exe_src ControlUnitExample.cpp
                  RTWorkerCU.cpp
                  SCWorkerCU.cpp
                  SinWaveCommand.cpp
                  TestCorrelatingCommand.cpp
                  TestSetOnly.cpp
                  SinGeneratorOpcodeLogic.cpp
                  SinGeneratorDriver.cpp
                  SinGeneratorRemoteDriver.cpp)
SET(roc_test_stress_exe_src StressRpcTest/main.cpp)
SET(eu_api_plugin_example_src EUApiTestPlugin/EUApiTestPlugin.cpp)
SET(sin_gen_ext_drv SinGeneratorExternalDriver/main.cpp)


ADD_EXECUTABLE(ExampleControlUnit ${cutest_exe_src})
#ADD_EXECUTABLE(RpcTestStress ${roc_test_stress_exe_src})
#ADD_EXECUTABLE(SinGeneratorExternalDriver ${sin_gen_ext_drv})

#ADD_LIBRARY(EUApiTestPlugin SHARED ${eu_api_plugin_example_src})
#set_target_properties(EUApiTestPlugin PROPERTIES PREFIX "" SUFFIX ".chaos_extension")


TARGET_LINK_LIBRARIES(ExampleControlUnit ${FrameworkLib})
#TARGET_LINK_LIBRARIES(RpcTestStress pthread chaos_uitoolkit chaos_common ${FrameworkLib})
#TARGET_LINK_LIBRARIES(EUApiTestPlugin)
#TARGET_LINK_LIBRARIES(SinGeneratorExternalDriver pthread chaos_micro_unit_toolkit)


INSTALL(TARGETS ExampleControlUnit
        DESTINATION "bin"
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
#INSTALL_TARGETS(/bin SinGeneratorExternalDriver)
#INSTALL_TARGETS(/bin RpcTestStress)
