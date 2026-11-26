cmake_minimum_required(VERSION 3.20)

project(InstPushButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstPushButton main_InstPushButton.cpp)

target_link_libraries(InstPushButton PRIVATE X11::X11)
