cmake_minimum_required(VERSION 3.20)

project(TValueListEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TValueListEditor main_TValueListEditor.cpp)

target_link_libraries(TValueListEditor PRIVATE X11::X11)
