cmake_minimum_required(VERSION 3.20)

project(TFilterComboBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TFilterComboBox main_TFilterComboBox.cpp)

target_link_libraries(TFilterComboBox PRIVATE X11::X11)
