# Automatically generate moc files for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 COMPONENTS Core Gui Widgets Qml Quick REQUIRED CONFIG)
if(Qt5Widgets_FOUND)
	message("Using Qt ${Qt5Widgets_VERSION} from ${Qt5Widgets_INCLUDE_DIRS}")
endif()

add_compile_definitions(SHARE_DIR="${GOGH_SHARE_DIR}")

set(SRC
	utils/fileutils.h
	utils/fileutils.cpp
	utils/strutils.h
	utils/strutils.cpp
	Logger.h
	Logger.cpp
	Variant.h
	Signal.h
	ParameterType.h
	Parameter.h
	Parameter.cpp
	main.cpp
)

set(RESOURCES
	resources.qrc
)

add_executable(
	Gogh
	${SRC}
	${RESOURCES}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries_and_dll(Gogh Qt5::Core)
target_link_libraries_and_dll(Gogh Qt5::Gui)
target_link_libraries_and_dll(Gogh Qt5::Widgets)
target_link_libraries_and_dll(Gogh Qt5::Qml)
target_link_libraries_and_dll(Gogh Qt5::Quick)
target_link_libraries_and_dll(Gogh Qt5::Network)

# For IDE projects
group_source_by_folder(${SRC})
