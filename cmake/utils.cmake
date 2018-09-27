
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
