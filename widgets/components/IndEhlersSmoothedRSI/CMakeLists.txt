cmake_minimum_required(VERSION 3.20)

project(IndEhlersSmoothedRSI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndEhlersSmoothedRSI main_IndEhlersSmoothedRSI.cpp)

target_link_libraries(IndEhlersSmoothedRSI PRIVATE X11::X11)
