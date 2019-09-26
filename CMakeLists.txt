cmake_minimum_required(VERSION 3.10)
project(qnapctrl)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(qnapd)
add_subdirectory(qnapctl)
