cmake_minimum_required(VERSION 3.6)
project(Gogh)

set(CMAKE_CXX_STANDARD 11)

include(cmake/utils.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(src)

# Define Gogh as startup project in VS
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Gogh)

#file(COPY share DESTINATION .)
