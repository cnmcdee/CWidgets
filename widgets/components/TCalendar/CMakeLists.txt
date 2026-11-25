cmake_minimum_required(VERSION 3.20)

project(TCalendar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TCalendar main_TCalendar.cpp)

target_link_libraries(TCalendar PRIVATE X11::X11)
