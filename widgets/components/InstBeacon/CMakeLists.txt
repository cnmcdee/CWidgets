cmake_minimum_required(VERSION 3.20)

project(InstBeacon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstBeacon main_InstBeacon.cpp)

target_link_libraries(InstBeacon PRIVATE X11::X11)
