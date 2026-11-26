cmake_minimum_required(VERSION 3.20)

project(InstRockerSwitchIlluminated LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstRockerSwitchIlluminated main_InstRockerSwitchIlluminated.cpp)

target_link_libraries(InstRockerSwitchIlluminated PRIVATE X11::X11)
