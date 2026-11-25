cmake_minimum_required(VERSION 3.20)

project(IndEfficiencyRatio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndEfficiencyRatio main_IndEfficiencyRatio.cpp)

target_link_libraries(IndEfficiencyRatio PRIVATE X11::X11)
