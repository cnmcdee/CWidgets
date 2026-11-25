cmake_minimum_required(VERSION 3.20)

project(CWidgets LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

# Test build: warnings as visible as possible (added by test agent).
add_compile_options(-Wall -Wextra)

# Build every component demo. Each component dir ships its own
# CMakeLists.txt and builds standalone (copy the dir out on its own and
# run cmake -S . -B build); the top level just adds each one as a
# subdirectory, one executable target per component.
file(GLOB COMPONENT_DIRS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/components"
     "${CMAKE_CURRENT_SOURCE_DIR}/components/*")
foreach(dir IN LISTS COMPONENT_DIRS)
  if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/components/${dir}")
    add_subdirectory("components/${dir}")
  endif()
endforeach()
