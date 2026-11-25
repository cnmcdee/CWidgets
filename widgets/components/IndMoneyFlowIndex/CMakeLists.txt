cmake_minimum_required(VERSION 3.20)

project(IndMoneyFlowIndex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndMoneyFlowIndex main_IndMoneyFlowIndex.cpp)

target_link_libraries(IndMoneyFlowIndex PRIVATE X11::X11)
