cmake_minimum_required(VERSION 3.20)

project(InstPushButtonLatching LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstPushButtonLatching main_InstPushButtonLatching.cpp)

target_link_libraries(InstPushButtonLatching PRIVATE X11::X11)
