cmake_minimum_required(VERSION 3.20)

project(TPairSplitter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TPairSplitter main_TPairSplitter.cpp)

target_link_libraries(TPairSplitter PRIVATE X11::X11)
