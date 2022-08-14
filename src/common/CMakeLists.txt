include_directories(include)

add_library(type 		OBJECT type.c)
add_library(core 		OBJECT core.c)
add_library(comparator	OBJECT comparator.c)

set(LIB_OBJECTS)

list(APPEND LIB_OBJECTS
	$<TARGET_OBJECTS:type>
	$<TARGET_OBJECTS:core>
	$<TARGET_OBJECTS:comparator>)

add_library(common
	STATIC
	${LIB_OBJECTS})

target_include_directories(common INTERFACE include)
