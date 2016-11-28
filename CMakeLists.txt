cmake_minimum_required (VERSION 2.8)

SET(CMAKE_CXX_COMPILER g++-5)

project (ts)
if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -Wno-reorder")
endif()

add_subdirectory (test)
add_subdirectory (demo)
enable_testing ()

