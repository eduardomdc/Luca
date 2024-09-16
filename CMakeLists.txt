cmake_minimum_required(VERSION 3.26)
project(chat)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(MY_SOURCES
    ../src/main.cpp
    ../src/client.cpp
    ../src/interface.cpp
)
set_source_files_properties(${MY_SOURCES} PROPERTIES COMPILE_OPTIONS -Wall)

add_executable(chat ${MY_SOURCES})

target_link_libraries(chat ${CURSES_LIBRARIES})