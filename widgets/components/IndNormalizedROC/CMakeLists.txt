cmake_minimum_required(VERSION 3.20)

project(IndNormalizedROC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndNormalizedROC main_IndNormalizedROC.cpp)

target_link_libraries(IndNormalizedROC PRIVATE X11::X11)
