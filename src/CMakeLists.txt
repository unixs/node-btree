set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(CheckGLIB)

include_directories(include)

add_subdirectory(common)
add_subdirectory(btree)
add_subdirectory(glib_version)

add_library(${PROJECT_NAME}
	SHARED
	init.c)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	PREFIX ""
	SUFFIX ".node")

target_link_libraries(${PROJECT_NAME}
	PRIVATE
	common
	btree
	glib_version
	${CMAKE_JS_LIB})
