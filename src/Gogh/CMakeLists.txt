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
	NodeGraphModel.h
	NodeGraphModel.cpp
	NodeGraphScene.h
	NodeGraphScene.cpp
	NodeParmListView.h
	NodeParmListView.cpp
	RenderDialog.h
	RenderDialog.cpp

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
	Nodes/CodecNode.h
	Nodes/CodecNode.ui
	Nodes/CodecNode.cpp
	Nodes/MixNode.h
	Nodes/MixNode.ui
	Nodes/MixNode.cpp

	Tools/AbstractGraphTool.h
	Tools/PanTool.h
	Tools/PanTool.cpp
	Tools/CutTool.h
	Tools/CutTool.cpp

	EnvModel.h
	EnvModel.cpp
	Dialogs/EnvDialog.h
	Dialogs/EnvDialog.ui
	Dialogs/EnvDialog.cpp

	main.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries_and_dll(Gogh Qt5::Core)
target_link_libraries_and_dll(Gogh Qt5::Gui)
target_link_libraries_and_dll(Gogh Qt5::Widgets)
