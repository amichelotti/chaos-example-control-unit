cmake_minimum_required(VERSION 2.8)
include(${CMAKE_SOURCE_DIR}/../../chaosframework/config/CMakeChaos.txt)

SET(cutest_exe_src ControlUnitExample.cpp RTWorkerCU.cpp SCWorkerCU.cpp SinWaveCommand.cpp TestCorrelatingCommand.cpp SinGeneratorDriver.cpp)
SET(roc_test_stress_exe_src StressRpcTest/main.cpp)
SET(eu_api_plugin_example_src EUApiTestPlugin/EUApiTestPlugin.cpp)


ADD_EXECUTABLE(ExampleControlUnit ${cutest_exe_src})
ADD_EXECUTABLE(RpcTestStress ${roc_test_stress_exe_src})
ADD_LIBRARY(EUApiTestPlugin SHARED ${eu_api_plugin_example_src})
set_target_properties(EUApiTestPlugin PROPERTIES PREFIX "" SUFFIX ".chaos_extension")

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ELSE()
ENDIF()


TARGET_LINK_LIBRARIES(ExampleControlUnit pthread chaos_cutoolkit chaos_common ${FrameworkLib})
TARGET_LINK_LIBRARIES(RpcTestStress pthread chaos_uitoolkit chaos_common ${FrameworkLib})
TARGET_LINK_LIBRARIES(EUApiTestPlugin)

INSTALL_TARGETS(/bin ExampleControlUnit)
INSTALL_TARGETS(/bin RpcTestStress)
