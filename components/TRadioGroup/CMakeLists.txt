cmake_minimum_required(VERSION 3.20)

project(TRadioGroup LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TRadioGroup main_TRadioGroup.cpp)

target_link_libraries(TRadioGroup PRIVATE X11::X11)
