cmake_minimum_required(VERSION 3.20)

project(XSpreadsheet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(XSpreadsheet main_XSpreadsheet.cpp)

target_link_libraries(XSpreadsheet PRIVATE X11::X11)
