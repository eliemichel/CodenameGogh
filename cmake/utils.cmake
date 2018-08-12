
# This macro is a container for dirty search expressions, it just has to work
macro(guess_qt5_dir)
	if(NOT DEFINED Qt5_DIR)

		set(hint_dirs
			"C:/Qt" # Windows
			"D:/Qt" # Windows
			"E:/Qt" # Windows
			"$ENV{HomeDrive}$ENV{HomePath}/Qt" # Windows - user home installation
			"$ENV{HOME}/Qt" # Unix - user home installation
			"/usr/local/Cellar/qt5/" # OSX - homebrew
		)
		set(qt_roots "")
		foreach(dir ${hint_dirs})
			if(IS_DIRECTORY "${dir}")
				file(GLOB ls RELATIVE "${dir}" "${dir}/[0-9].*")
				foreach(file ${ls})
					if(IS_DIRECTORY "${dir}/${file}" AND "${file}" MATCHES "5\.[0-9]+.[0-9]+")
						list(APPEND qt_roots "${dir}/${file}")
					endif()
				endforeach()
			endif()
		endforeach()

		set(qt_build "")
		if(MSVC)
			if(MSVC_TOOLSET_VERSION EQUAL "120")
				set(qt_build "msvc2013")
			elseif(MSVC_TOOLSET_VERSION EQUAL "140")
				set(qt_build "msvc2015")
			elseif(MSVC_TOOLSET_VERSION EQUAL "141")
				set(qt_build "msvc2017")
			endif()
			if(CMAKE_SIZEOF_VOID_P EQUAL 8)
				set(qt_build "${qt_build}_64")
			endif()
		elseif(MINGW)
			set(qt_build "mingw53_32")
		endif()

		foreach(qt ${qt_roots})
			set(dir "${qt}/${qt_build}/lib/cmake/Qt5")
			if(IS_DIRECTORY "${dir}")
				set(Qt5_DIR "${dir}")
			endif()
		endforeach()
		
		# TODO: use qmake to locate Qt?

	endif()
endmacro()

# For Qt libraries
function(target_link_libraries_and_dll target lib)
	target_link_libraries(${target} ${lib})
	add_custom_command(
	    TARGET ${target} POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E copy_if_different
	        $<TARGET_FILE:${lib}>
	        $<TARGET_FILE_DIR:${target}>
	)
endfunction()

# Reproduce the original folder layout in IDE
function(group_source_by_folder)
	foreach(file ${ARGV}) 
		# Get the directory of the source file
		get_filename_component(parent_dir "${file}" DIRECTORY)

		# Remove common directory prefix to make the group
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group "${parent_dir}")

		# Make sure we are using windows slashes
		string(REPLACE "/" "\\" group "${group}")

		# Group into "Source Files" and "Header Files"
		if ("${file}" MATCHES ".*\\.cpp")
		   set(group "Source Files\\${group}")
		elseif("${file}" MATCHES ".*\\.h")
		   set(group "Header Files\\${group}")
		endif()

		source_group("${group}" FILES "${file}")
	endforeach()
endfunction()
