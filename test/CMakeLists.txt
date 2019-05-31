
add_compile_definitions(SHARE_DIR="${GOGH_SHARE_DIR}")

set(GOGH_SRC
	../src/Gogh/utils/fileutils.h
	../src/Gogh/utils/fileutils.cpp
	../src/Gogh/utils/strutils.h
	../src/Gogh/utils/strutils.cpp
	../src/Gogh/Logger.h
	../src/Gogh/Logger.cpp
	../src/Gogh/Variant.h
	../src/Gogh/Signal.h
	../src/Gogh/ParameterType.h
	../src/Gogh/Parameter.h
	../src/Gogh/Parameter.cpp
	../src/Gogh/Graph.h
)

set(SRC
	catch.hpp
	test_graph.cpp
)

add_executable(
	test_graph
	${GOGH_SRC}
	${SRC}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../src/Gogh")

# For IDE projects
group_source_by_folder(${SRC})

add_test(NAME TestGraph COMMAND test_graph)
