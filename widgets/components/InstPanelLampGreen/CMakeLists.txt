cmake_minimum_required(VERSION 3.20)

project(InstPanelLampGreen LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstPanelLampGreen main_InstPanelLampGreen.cpp)

target_link_libraries(InstPanelLampGreen PRIVATE X11::X11)
