cmake_minimum_required(VERSION 3.20)

project(GtkFlowBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkFlowBox main_GtkFlowBox.cpp)

target_link_libraries(GtkFlowBox PRIVATE X11::X11)
