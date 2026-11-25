cmake_minimum_required(VERSION 3.20)

project(IndFisherTransform LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndFisherTransform main_IndFisherTransform.cpp)

target_link_libraries(IndFisherTransform PRIVATE X11::X11)
