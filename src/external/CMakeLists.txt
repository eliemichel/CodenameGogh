# Glad (v0.1.20), providing headers for OpenGL procedures
set(GLAD_PROFILE "core" CACHE STRING "" FORCE)
set(GLAD_API "gl=,gles2=3.2" CACHE STRING "" FORCE)
add_subdirectory(glad)
set_property(TARGET glad PROPERTY FOLDER "External")

# GLFW (3.2.1), for openning the window and getting mouse/keyboard events
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
set_property(TARGET glfw PROPERTY FOLDER "External")

# NanoVG
file(GLOB nanovg_SRC
    "nanovg/src/*.c"
    "nanovg/src/*.h"
)
add_library(nanovg ${nanovg_SRC})
target_include_directories(nanovg PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src")
set_property(TARGET nanovg PROPERTY FOLDER "External")

# tiny-process-library
add_subdirectory(tiny-process-library)
set_property(TARGET tiny-process-library PROPERTY FOLDER "External")
