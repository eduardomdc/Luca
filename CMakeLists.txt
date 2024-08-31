cmake_minimum_required(VERSION 3.26)
project(chat)

set(MY_SOURCES
    ../src/main.cpp
    ../src/client.cpp
    ../src/chat.cpp
)

add_executable(chat ${MY_SOURCES})
