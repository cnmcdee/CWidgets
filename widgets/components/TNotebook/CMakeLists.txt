cmake_minimum_required(VERSION 3.20)

project(TNotebook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TNotebook main_TNotebook.cpp)

target_link_libraries(TNotebook PRIVATE X11::X11)
