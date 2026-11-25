cmake_minimum_required(VERSION 3.20)

project(IndTTMSqueeze LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndTTMSqueeze main_IndTTMSqueeze.cpp)

target_link_libraries(IndTTMSqueeze PRIVATE X11::X11)
