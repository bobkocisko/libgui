
# Define the packages required from the operating system
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
if (NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL not found")
endif ()
find_package(GLEW REQUIRED)
if (NOT GLEW_FOUND)
    message(FATAL_ERROR "GLEW not found")
endif ()
find_package(Freetype REQUIRED)
if (NOT FREETYPE_FOUND)
    message(FATAL_ERROR "Freetype not found")
endif ()
find_package(X11 REQUIRED)
if (NOT X11_FOUND)
    message(FATAL_ERROR "X11 not found")
endif ()

find_package(glfw3 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIR})


# External project freetypegl

externalproject_add(freetypegl
    GIT_REPOSITORY https://github.com/rougier/freetype-gl.git
    GIT_TAG f7e87d447fe89f002bfbf1571dbe8828dad6056b
    CMAKE_ARGS -Dfreetype-gl_BUILD_DEMOS:BOOL=OFF -Dfreetype-gl_BUILD_APIDOC:BOOL=OFF -Dfreetype-gl_BUILD_MAKEFONT:BOOL=OFF
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target freetype-gl
    INSTALL_COMMAND "" # No install command
    )
externalproject_get_property(freetypegl BINARY_DIR SOURCE_DIR)
add_library(libfreetypegl STATIC IMPORTED)
set_target_properties(libfreetypegl PROPERTIES
    IMPORTED_LOCATION ${BINARY_DIR}/libfreetype-gl.a
    )
include_directories(${SOURCE_DIR})
add_dependencies(libfreetypegl freetypegl)
set(FREETYPE_GL_SHADER_PATH ${SOURCE_DIR}/shaders)
set(FREETYPE_GL_FONT_PATH ${SOURCE_DIR}/fonts)

# Set up the src test runner executable

set(SOURCE_FILES
    ItemsViewModel.cpp
    ItemViewModel.cpp
    main.cpp
    include/ItemsViewModel.h
    include/ItemViewModel.h
    )

add_executable(libgui.sample.opengl ${SOURCE_FILES})
target_link_libraries(libgui.sample.opengl libgui libfreetypegl ${GLFW3_LIBRARY}
    ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${GLEW_LIBRARIES})

macro(deploy_freetype_gl_shader SHADER_FILE)
    set(SRC_FILE ${FREETYPE_GL_SHADER_PATH}/${SHADER_FILE})
    set(DEST_FILE $<TARGET_FILE_DIR:libgui.sample.opengl>/shaders/${SHADER_FILE})
    add_custom_command(
        TARGET libgui.sample.opengl POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_if_different "${SRC_FILE}" "${DEST_FILE}"
        COMMENT "Copying freetype-gl's ${SHADER_FILE} to sample build directory"
    )
endmacro()

deploy_freetype_gl_shader(text.vert)
deploy_freetype_gl_shader(text.frag)
deploy_freetype_gl_shader(v3f-c4f.vert)
deploy_freetype_gl_shader(v3f-c4f.frag)

macro(deploy_freetype_font FONT_FILE)
    set(SRC_FILE ${FREETYPE_GL_FONT_PATH}/${FONT_FILE})
    set(DEST_FILE $<TARGET_FILE_DIR:libgui.sample.opengl>/fonts/${FONT_FILE})
    add_custom_command(
        TARGET libgui.sample.opengl POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_if_different "${SRC_FILE}" "${DEST_FILE}"
        COMMENT "Copying font ${FONT_FILE} to sample build directory"
    )
endmacro()

deploy_freetype_font(Vera.ttf)