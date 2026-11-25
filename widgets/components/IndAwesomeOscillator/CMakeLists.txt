cmake_minimum_required(VERSION 3.20)

project(IndAwesomeOscillator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndAwesomeOscillator main_IndAwesomeOscillator.cpp)

target_link_libraries(IndAwesomeOscillator PRIVATE X11::X11)
