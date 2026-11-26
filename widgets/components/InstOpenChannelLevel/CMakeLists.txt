cmake_minimum_required(VERSION 3.20)

project(InstOpenChannelLevel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstOpenChannelLevel main_InstOpenChannelLevel.cpp)

target_link_libraries(InstOpenChannelLevel PRIVATE X11::X11)
