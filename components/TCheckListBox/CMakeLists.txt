cmake_minimum_required(VERSION 3.20)

project(TCheckListBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TCheckListBox main_TCheckListBox.cpp)

target_link_libraries(TCheckListBox PRIVATE X11::X11)
