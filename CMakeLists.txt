cmake_minimum_required(VERSION 3.1)

project(node_btree
	LANGUAGES C
	VERSION 0.6.2
	HOMEPAGE_URL https://github.com/unixs/node-btree)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS_DEBUG "-pipe -Wall -Wextra -Wpedantic -O0 -ggdb")
set(CMAKE_C_FLAGS_RELEASE "-pipe -march=native -O3")
set(CMAKE_C_FLAGS "-Wno-unused-parameter")

include(NodeJS.cmake)
nodejs_init()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

add_subdirectory(src)
