include(ExternalProject)

# We need threads support for the google test library
find_package(Threads REQUIRED)

set(SOURCE_FILES
    include/Common.h
    ButtonTests.cpp
    ElementManagerTests.cpp
    ElementTests.cpp
    main.cpp SliderTests.cpp TypesTest.cpp StateMachineTests.cpp
    StateMachine2Tests.cpp
    StateMachine3Tests.cpp
    IntersectionStackTests.cpp)

# External projects Google Test & Google Mock

externalproject_add(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG d945d8c000a0ade73585d143532266968339bbb3
    CONFIGURE_COMMAND cmake <SOURCE_DIR>/googlemock # gmock depends on gtest so both will get built
    INSTALL_COMMAND "" # There is no install target
)
externalproject_get_property(googletest SOURCE_DIR BINARY_DIR)

# Set up the gtest library from the external source

add_library(libgoogletest IMPORTED STATIC GLOBAL)
add_dependencies(libgoogletest googletest)
set_target_properties(libgoogletest PROPERTIES
    IMPORTED_LOCATION "${BINARY_DIR}/gtest/libgtest.a"
    IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}"
)
include_directories("${SOURCE_DIR}/googletest/include")

# Set up the gmock library from the external source

add_library(libgooglemock IMPORTED STATIC GLOBAL)
add_dependencies(libgooglemock libgoogletest)
set_target_properties(libgooglemock PROPERTIES
    IMPORTED_LOCATION "${BINARY_DIR}/libgmock.a"
    IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}"
)
include_directories("${SOURCE_DIR}/googlemock/include")


# We need Boost MSM (meta state machine)
find_package(Boost
    1.54.0
    REQUIRED
    )
include_directories(${Boost_INCLUDE_DIRS})


# Set up the src test runner executable

add_executable(libgui.test ${SOURCE_FILES})
target_link_libraries(libgui.test libgoogletest libgooglemock libgui)