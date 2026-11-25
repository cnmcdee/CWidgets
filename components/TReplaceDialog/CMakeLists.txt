cmake_minimum_required(VERSION 3.20)

project(TReplaceDialog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TReplaceDialog main_TReplaceDialog.cpp)

target_link_libraries(TReplaceDialog PRIVATE X11::X11)
