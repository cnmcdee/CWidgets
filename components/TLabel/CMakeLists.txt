cmake_minimum_required(VERSION 3.20)

project(TLabel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TLabel main_TLabel.cpp)

target_link_libraries(TLabel PRIVATE X11::X11)
