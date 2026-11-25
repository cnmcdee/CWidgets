cmake_minimum_required(VERSION 3.20)

project(TButtonPanel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TButtonPanel main_TButtonPanel.cpp)

target_link_libraries(TButtonPanel PRIVATE X11::X11)
