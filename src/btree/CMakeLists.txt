include_directories(include)
link_libraries(common)

add_library(btree_export		OBJECT btree.c)
add_library(constructor			OBJECT constructor.c)
add_library(array				OBJECT array.c)
add_library(map					OBJECT map.c)
add_library(specific			OBJECT specific.c)
add_library(static				OBJECT static.c)

set(LIB_OBJECTS)

list(APPEND LIB_OBJECTS
	$<TARGET_OBJECTS:btree_export>
	$<TARGET_OBJECTS:constructor>
	$<TARGET_OBJECTS:array>
	$<TARGET_OBJECTS:map>
	$<TARGET_OBJECTS:specific>
	$<TARGET_OBJECTS:static>)

if(HAS_GTREE_NODE)
	add_library(iterators 			OBJECT iterators.c)
	add_library(search				OBJECT search.c)
	add_library(conversion			OBJECT conversion.c)

	list(APPEND LIB_OBJECTS $<TARGET_OBJECTS:iterators>)
	list(APPEND LIB_OBJECTS $<TARGET_OBJECTS:search>)
	list(APPEND LIB_OBJECTS $<TARGET_OBJECTS:conversion>)
endif()

add_library(btree
	STATIC
	${LIB_OBJECTS})
