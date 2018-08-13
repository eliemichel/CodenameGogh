# Automatically generate moc files for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED CONFIG)
if(Qt5Widgets_FOUND)
	message("Using Qt ${Qt5Widgets_VERSION} from ${Qt5Widgets_INCLUDE_DIRS}")
endif()

add_compile_definitions(SHARE_DIR="${GOGH_SHARE_DIR}")

set(SRC
	resources.qrc
	Logger.h
	Logger.cpp
	utils/strutils.h
	utils/strutils.cpp
	utils/fileutils.h
	utils/fileutils.cpp
	utils/stringlist.h
	utils/stringlist.cpp
	Node.h
	Node.cpp
	NodeType.h
	NodeType.cpp
	Parameter.h
	Parameter.cpp

	DefaultNodeEditor.h
	DefaultNodeEditor.cpp
	MainWindow.h
	MainWindow.ui
	MainWindow.cpp
	NodeGraphView.h
	NodeGraphView.cpp
	NodeGraphModel.h
	NodeGraphModel.cpp
	NodeGraphScene.h
	NodeGraphScene.cpp
	NodeParmListView.h
	NodeParmListView.cpp
	RenderProcess.h
	RenderProcess.cpp
	FileProbeProcess.h
	FileProbeProcess.cpp

	LinkGraphicsItem.h
	LinkGraphicsItem.cpp
	NodeGraphicsItem.h
	NodeGraphicsItem.cpp
	SlotGraphicsItem.h
	SlotGraphicsItem.cpp
	SlotIndex.h

	Nodes/OutputNode.h
	Nodes/OutputNode.cpp
	Nodes/OutputNodeEditor.h
	Nodes/OutputNodeEditor.cpp
	Nodes/InputNode.h
	Nodes/InputNode.cpp
	Nodes/ScaleNode.h
	Nodes/ScaleNode.cpp
	Nodes/CodecNode.h
	Nodes/CodecNode.cpp
	Nodes/MixNode.h
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
	Dialogs/RenderDialog.h
	Dialogs/RenderDialog.cpp

	Ui/UiBase.h
	Ui/UiBoxLayout.inc.h
	Ui/UiApp.h
	Ui/UiApp.cpp
	Ui/UiWindow.h
	Ui/UiWindow.cpp

	main.cpp
)

add_executable(
	Gogh
	${SRC}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries_and_dll(Gogh Qt5::Core)
target_link_libraries_and_dll(Gogh Qt5::Gui)
target_link_libraries_and_dll(Gogh Qt5::Widgets)

target_link_libraries(Gogh glad)
target_link_libraries(Gogh glfw)
target_link_libraries(Gogh nanovg)

# For IDE projects
group_source_by_folder(${SRC})
