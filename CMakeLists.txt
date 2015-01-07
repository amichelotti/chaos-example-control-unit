cmake_minimum_required(VERSION 2.6)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)

SET(cutest_exe_src ControlUnitExample.cpp RTWorkerCU.cpp SCWorkerCU.cpp SinWaveCommand.cpp TestCorrelatingCommand.cpp DummyDriver.cpp)
SET(roc_test_stress_exe_src StressRpcTest/main.cpp)



ADD_EXECUTABLE(ExampleControlUnit ${cutest_exe_src})
ADD_EXECUTABLE(RpcTestStress ${roc_test_stress_exe_src})

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ELSE()
ENDIF()


TARGET_LINK_LIBRARIES(ExampleControlUnit pthread chaos_cutoolkit chaos_common ${FrameworkLib})
TARGET_LINK_LIBRARIES(RpcTestStress pthread chaos_uitoolkit chaos_common ${FrameworkLib})

INSTALL_TARGETS(/bin ExampleControlUnit)
INSTALL_TARGETS(/bin RpcTestStress)