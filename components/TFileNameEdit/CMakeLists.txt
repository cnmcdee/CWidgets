cmake_minimum_required(VERSION 3.20)

project(TFileNameEdit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TFileNameEdit main_TFileNameEdit.cpp)

target_link_libraries(TFileNameEdit PRIVATE X11::X11)
