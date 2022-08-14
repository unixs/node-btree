include(CheckSymbolExists)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB2 REQUIRED glib-2.0)

include_directories(${GLIB2_INCLUDE_DIRS})
link_libraries(${GLIB2_LDFLAGS})

list(APPEND CMAKE_REQUIRED_LIBRARIES ${GLIB2_LDFLAGS})
list(APPEND CMAKE_REQUIRED_INCLUDES ${GLIB2_INCLUDE_DIRS})

check_symbol_exists(g_tree_remove_all "glib.h" HAS_GTREE_REMOVE_ALL)
check_symbol_exists(g_tree_lookup_node "glib.h" HAS_GTREE_NODE)

if(HAS_GTREE_REMOVE_ALL)
	add_compile_definitions(HAS_GTREE_REMOVE_ALL)
endif()

if(HAS_GTREE_NODE)
	add_compile_definitions(HAS_GTREE_NODE)
else()
	message(WARNING
		"Available GLib is not support direct node access.
		Version < 2.68.")
endif()
