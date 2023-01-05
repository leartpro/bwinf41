cmake_minimum_required(VERSION 3.24)
project(bwinf41)

set(CMAKE_CXX_STANDARD 23)

add_executable(bwinf41 LennartProtte/Aufgabe1-Implementierung/main.cpp LennartProtte/Aufgabe1-Implementierung/main.h)
#[===[
add_custom_command(
        TARGET ${/bwinf41} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${LennartProtte/Aufgabe1-Implementierung/main.cpp}/input.txt
        ${/cmake-build-debug}/input.txt)
]===]
