cmake_minimum_required(VERSION 3.20)

project(TLabeledEdit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TLabeledEdit main_TLabeledEdit.cpp)

target_link_libraries(TLabeledEdit PRIVATE X11::X11)
