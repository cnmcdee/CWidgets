cmake_minimum_required(VERSION 3.20)

project(InstNeedleTapered LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstNeedleTapered main_InstNeedleTapered.cpp)

target_link_libraries(InstNeedleTapered PRIVATE X11::X11)
