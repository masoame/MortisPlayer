cmake_minimum_required(VERSION 3.15)

project(cmake_template)

include(cmake/config.cmake)
include(cmake/package.cmake)

add_subdirectory(core)
add_subdirectory(gui) 
add_subdirectory(test)