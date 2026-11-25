cmake_minimum_required(VERSION 3.20)

project(IndROC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndROC main_IndROC.cpp)

target_link_libraries(IndROC PRIVATE X11::X11)
