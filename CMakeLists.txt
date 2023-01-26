cmake_minimum_required(VERSION 3.23)
project(task1)
project(task2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

add_executable(task1 LennartProtte/Aufgabe1-Implementierung/main.cpp)
add_executable(task2 LennartProtte/Aufgabe2-Implementierung/main.cpp)