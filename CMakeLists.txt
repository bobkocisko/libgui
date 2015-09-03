cmake_minimum_required(VERSION 3.3)
project(libgui)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# We need threads support at least for the google test library
find_package(Threads REQUIRED)

add_subdirectory(libgui.code)

# Make the include files from the library available to other targets
include_directories(libgui.code/include)

add_subdirectory(libgui.sample.windows)
add_subdirectory(libgui.sample.windows2)
add_subdirectory(libgui.test)
