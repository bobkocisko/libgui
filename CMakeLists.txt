cmake_minimum_required(VERSION 3.3)
project(libgui)

option(libgui_build_samples "Build all of libgui's own samples." OFF)
option(libgui_build_tests "Build all of libgui's own tests." OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Make the include files from the library available to all targets
include_directories(libgui.code/include)

add_subdirectory(libgui.code)

if(libgui_build_samples)
    add_subdirectory(libgui.sample.windows)
    add_subdirectory(libgui.sample.windows2)
endif()

if(libgui_build_tests)
    add_subdirectory(libgui.test)
endif()
