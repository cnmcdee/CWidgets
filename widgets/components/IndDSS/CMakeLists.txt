cmake_minimum_required(VERSION 3.20)

project(IndDSS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndDSS main_IndDSS.cpp)

target_link_libraries(IndDSS PRIVATE X11::X11)
