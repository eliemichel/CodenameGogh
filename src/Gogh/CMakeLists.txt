# Automatically generate moc files for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
if(Qt5Widgets_FOUND)
	message("Using Qt ${Qt5Widgets_VERSION} from ${Qt5Widgets_INCLUDE_DIRS}")
endif()

add_executable(
	Nodia
	resources.qrc
	Logger.h
	Logger.cpp
	utils/strutils.h
	utils/strutils.cpp
	utils/fileutils.h
	utils/fileutils.cpp
	MainWindow.h
	MainWindow.ui
	MainWindow.cpp
	NodeGraphView.h
	NodeGraphView.cpp
	main.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries_and_dll(Nodia Qt5::Core)
target_link_libraries_and_dll(Nodia Qt5::Gui)
target_link_libraries_and_dll(Nodia Qt5::Widgets)
