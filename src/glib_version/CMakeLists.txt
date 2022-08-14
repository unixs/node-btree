include_directories(include)
link_libraries(common)

add_library(glib_version_export OBJECT glib_version.c)

set(LIB_OBJECTS)

list(APPEND LIB_OBJECTS
	$<TARGET_OBJECTS:glib_version_export>)

add_library(glib_version
	STATIC
	${LIB_OBJECTS})
