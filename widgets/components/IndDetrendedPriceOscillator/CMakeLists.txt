cmake_minimum_required(VERSION 3.20)

project(IndDetrendedPriceOscillator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndDetrendedPriceOscillator main_IndDetrendedPriceOscillator.cpp)

target_link_libraries(IndDetrendedPriceOscillator PRIVATE X11::X11)
