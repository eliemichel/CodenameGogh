
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
	foreach(FILE ${ARGV}) 
		# Get the directory of the source file
		get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

		# Remove common directory prefix to make the group
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

		# Make sure we are using windows slashes
		string(REPLACE "/" "\\" GROUP "${GROUP}")

		# Group into "Source Files" and "Header Files"
		if ("${FILE}" MATCHES ".*\\.cpp")
		   set(GROUP "Source Files\\${GROUP}")
		elseif("${FILE}" MATCHES ".*\\.h")
		   set(GROUP "Header Files\\${GROUP}")
		endif()

		source_group("${GROUP}" FILES "${FILE}")
	endforeach()
endfunction()
