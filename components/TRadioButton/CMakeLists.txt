cmake_minimum_required(VERSION 3.20)

project(TRadioButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TRadioButton main_TRadioButton.cpp)

target_link_libraries(TRadioButton PRIVATE X11::X11)
