cmake_minimum_required(VERSION 3.20)

project(TFloatSpinEdit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TFloatSpinEdit main_TFloatSpinEdit.cpp)

target_link_libraries(TFloatSpinEdit PRIVATE X11::X11)
