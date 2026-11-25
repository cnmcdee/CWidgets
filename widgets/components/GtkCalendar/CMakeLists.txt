cmake_minimum_required(VERSION 3.20)

project(GtkCalendar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkCalendar main_GtkCalendar.cpp)

target_link_libraries(GtkCalendar PRIVATE X11::X11)
