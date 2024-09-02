cmake_minimum_required(VERSION 3.26)
project(chat)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(MY_SOURCES
    ../src/main.cpp
    ../src/client.cpp
    ../src/interface.cpp
)

add_executable(chat ${MY_SOURCES})

target_link_libraries(chat ${CURSES_LIBRARIES})