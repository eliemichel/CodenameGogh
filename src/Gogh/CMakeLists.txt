# Automatically generate moc files for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
if(Qt5Widgets_FOUND)
	message("Using Qt ${Qt5Widgets_VERSION} from ${Qt5Widgets_INCLUDE_DIRS}")
endif()

add_executable(
	Gogh
	resources.qrc
	Logger.h
	Logger.cpp
	utils/strutils.h
	utils/strutils.cpp
	utils/fileutils.h
	utils/fileutils.cpp
	Slot.h
	Slot.cpp
	MainWindow.h
	MainWindow.ui
	MainWindow.cpp
	NodeGraphView.h
	NodeGraphView.cpp
	NodeWidget.h
	NodeWidget.cpp
	LinkGraphicsItem.h
	LinkGraphicsItem.cpp
	NodeGraphicsItem.h
	NodeGraphicsItem.cpp
	SlotGraphicsItem.h
	SlotGraphicsItem.cpp
	Nodes/TestNode.h
	Nodes/TestNode.ui
	Nodes/TestNode.cpp
	Nodes/OutputNode.h
	Nodes/OutputNode.ui
	Nodes/OutputNode.cpp
	Nodes/InputNode.h
	Nodes/InputNode.ui
	Nodes/InputNode.cpp
	Nodes/ScaleNode.h
	Nodes/ScaleNode.ui
	Nodes/ScaleNode.cpp
	main.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries_and_dll(Gogh Qt5::Core)
target_link_libraries_and_dll(Gogh Qt5::Gui)
target_link_libraries_and_dll(Gogh Qt5::Widgets)
