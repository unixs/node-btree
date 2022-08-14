include(CheckCCompilerFlag)

message(STATUS "CPU: ${CMAKE_HOST_SYSTEM_PROCESSOR}")

set(CMAKE_C_FLAGS "-pipe")

CHECK_C_COMPILER_FLAG(-march=native HAVE_CC_MARCH_NATIVE)
if(HAVE_CC_MARCH_NATIVE)
	string(APPEND CMAKE_C_FLAGS " -march=native")
endif()
