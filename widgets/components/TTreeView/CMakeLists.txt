cmake_minimum_required(VERSION 3.20)

project(TTreeView LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TTreeView main_TTreeView.cpp)

target_link_libraries(TTreeView PRIVATE X11::X11)
