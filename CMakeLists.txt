cmake_minimum_required(VERSION 3.10)
project(
  crt-shader
  VERSION 0.0.1
  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE OCTREE_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp"
     "${CMAKE_SOURCE_DIR}/src/*.c")

find_package(PkgConfig REQUIRED)
find_package(fmt REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_executable(crt-shader ${OCTREE_SOURCES})

target_compile_definitions(crt-shader PRIVATE GLFW_INCLUDE_NONE)

target_include_directories(crt-shader PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(crt-shader PRIVATE glfw fmt::fmt)
