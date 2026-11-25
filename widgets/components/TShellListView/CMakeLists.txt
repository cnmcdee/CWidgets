cmake_minimum_required(VERSION 3.20)

project(TShellListView LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TShellListView main_TShellListView.cpp)

target_link_libraries(TShellListView PRIVATE X11::X11)
