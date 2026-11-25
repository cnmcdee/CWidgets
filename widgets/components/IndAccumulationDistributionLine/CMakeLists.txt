cmake_minimum_required(VERSION 3.20)

project(IndAccumulationDistributionLine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndAccumulationDistributionLine main_IndAccumulationDistributionLine.cpp)

target_link_libraries(IndAccumulationDistributionLine PRIVATE X11::X11)
