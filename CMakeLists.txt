cmake_minimum_required(VERSION 3.10)

# set the project name
project(Terminal)
add_compile_options(-std=gnu99)

set(SOURCES
    src/Process_Manager.c
    src/Command_Manager.c
    src/diff.c)

include_directories(include)

# add the executable
add_executable(${PROJECT_NAME} main.c ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)