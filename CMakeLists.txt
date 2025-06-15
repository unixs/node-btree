cmake_minimum_required(VERSION 3.22)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0091 NEW)

project(node_btree
	VERSION 0.9.0
	LANGUAGES C)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(CheckCPUFlags)
include(BuildConfig)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
	add_link_options(-undefined dynamic_lookup)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build/${CMAKE_BUILD_TYPE})

if(NOT DEFINED CMAKE_JS_INC)
	message(STATUS "CMAKE_JS_INC is not direct defined.")
	if(DEFINED ENV{CMAKE_JS_INC})
		message(STATUS "Pass CMAKE_JS_INC from env.")
		set(CMAKE_JS_INC $ENV{CMAKE_JS_INC})
	endif()

	if(NOT CMAKE_JS_INC)
		message(
			FATAL_ERROR
			"CMAKE_JS_INC is not defined. Try checking your CMake.js configuration.")
	endif()
endif()
message(STATUS "CMAKE_JS_INC: ${CMAKE_JS_INC}")

include_directories(${CMAKE_JS_INC})

add_subdirectory(src)

enable_testing()

set(COVERAGE_LINES_PERCENT 90)

add_test(
	NAME coverage
	COMMAND "gcovr" "-sp" "-r" ".." "--print-summary" "--fail-under-line" ${COVERAGE_LINES_PERCENT} ".")
