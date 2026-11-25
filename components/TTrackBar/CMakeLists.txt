cmake_minimum_required(VERSION 3.20)

project(TTrackBar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TTrackBar main_TTrackBar.cpp)

target_link_libraries(TTrackBar PRIVATE X11::X11)
