cmake_minimum_required(VERSION 3.10)

enable_language(C)

project(Connect)

# this remains from an attempt to try and launch the app with one Makefile command, but there is no command common between the 2 platforms that can do so
#if (WIN32)
#	# move the .exe output path from "/build/Debug" to "/build"
#	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG  "${CMAKE_CURRENT_SOURCE_DIR}/build")
#endif (WIN32)

add_executable(Connect structs/Stack.c structs/Hashmap.c structs/Matrix.c AI.c common/functions.c common/data.c game.c menu.c)

if (NOT WIN32)
	target_link_libraries(Connect m)
endif (NOT WIN32)