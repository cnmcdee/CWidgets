cmake_minimum_required(VERSION 3.20)

project(TProgressBar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TProgressBar main_TProgressBar.cpp)

target_link_libraries(TProgressBar PRIVATE X11::X11)
