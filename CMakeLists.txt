cmake_minimum_required(VERSION 3.12)
project(Gogh)

set(CMAKE_CXX_STANDARD 11)

include(cmake/utils.cmake)

option(GOGH_BUILD_DOCS "Build the Gogh developper documentation" ON)

set(GOGH_SHARE_DIR "${Gogh_SOURCE_DIR}/share")

guess_qt5_dir()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Gogh binary

add_subdirectory(src)

# Documentation

if (GOGH_BUILD_DOCS)
    find_package(Doxygen)
	if (DOXYGEN_FOUND)
	    add_custom_target(
	    	docs ALL "${DOXYGEN_EXECUTABLE} doc/doxygen.conf"
			WORKING_DIRECTORY "${Gogh_SOURCE_DIR}"
			COMMENT "Generating developper documentation in doc/doxygen" VERBATIM
		)
	endif()
endif()

# Define Gogh as startup project in VS
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Gogh)

#file(COPY share DESTINATION .)
