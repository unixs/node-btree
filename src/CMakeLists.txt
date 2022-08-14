set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB2 REQUIRED glib-2.0)

include_directories(include ${GLIB2_INCLUDE_DIRS} ${NODEJS_INCLUDE_DIRS})

add_library(constructor	OBJECT constructor.c)
add_library(common		OBJECT common.c)
add_library(array		OBJECT array.c)
add_library(map			OBJECT map.c)
add_library(specific	OBJECT specific.c)
add_library(static		OBJECT static.c)
add_library(iterators	OBJECT iterators.c)
add_library(extra		OBJECT extra.c)

add_nodejs_module(${PROJECT_NAME} init.c)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
	${GLIB2_LDFLAGS})
