cmake_minimum_required(VERSION 3.10)

# set the project name
project(Connect)

# add the executable
add_executable(Connect structs/Stack.c structs/Hashmap.c structs/Matrix.c AI.c common/functions.c common/data.c game.c menu.c)

target_link_libraries(Connect m)
