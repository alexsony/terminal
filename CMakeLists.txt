cmake_minimum_required(VERSION 3.10)

# set the project name
project(Terminal)

set(SOURCES)

include_directories(include)

# add the executable
add_executable(${PROJECT_NAME} main.c ${SOURCES})
target_link_libraries(${PROJECT_NAME} ncurses)
target_include_directories(${PROJECT_NAME} PUBLIC include)