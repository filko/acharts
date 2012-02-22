cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

project(Charts)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++0x")

find_package(Asciidoc REQUIRED)
if(NOT ASCIIDOC_EXECUTABLE)
  message(FATAL_ERROR "Asciidoc not found!")
endif()

add_subdirectory(doc)
add_subdirectory(src)
