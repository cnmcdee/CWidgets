cmake_minimum_required(VERSION 3.20)

project(TPageSetupDialog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TPageSetupDialog main_TPageSetupDialog.cpp)

target_link_libraries(TPageSetupDialog PRIVATE X11::X11)
