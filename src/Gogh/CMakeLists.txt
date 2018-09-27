
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
	ArgParse.h
	ArgParse.cpp
	mainGui.h
	mainGui.cpp
	Graph.h
	Graph.cpp
	Node.h
	Node.cpp
	Link.h
	Link.cpp
	Slot.h
	Slot.cpp
	NodeType.h
	NodeType.cpp
	Parameter.h
	Parameter.cpp
	RenderCommand.h
	RenderCommand.cpp
	MainEventLoop.h
	MainEventLoop.cpp

	Object.h
	RenderProcess.h
	RenderProcess.cpp
	FileProbeProcess.h
	FileProbeProcess.cpp

	Nodes/OutputNode.h
	Nodes/OutputNode.cpp
	Nodes/InputNode.h
	Nodes/InputNode.cpp
	Nodes/ScaleNode.h
	Nodes/ScaleNode.cpp
	Nodes/CodecNode.h
	Nodes/CodecNode.cpp
	Nodes/MixNode.h
	Nodes/MixNode.cpp
	Nodes/FramerateNode.h
	Nodes/FramerateNode.cpp

	Variant.h

	Ui/UiRect.h
	Ui/UiBase.h
	Ui/UiBoxLayout.inc.h
	Ui/UiTextInput.h
	Ui/UiTextInput.cpp
	Ui/UiIntInput.h
	Ui/UiIntInput.cpp
	Ui/UiEnumInput.h
	Ui/UiEnumInput.cpp
	Ui/UiButton.h
	Ui/UiButton.cpp
	Ui/UiCheckbox.h
	Ui/UiCheckbox.cpp
	Ui/UiMenu.h
	Ui/UiContextMenu.h
	Ui/UiContextMenu.cpp
	Ui/NodeArea.h
	Ui/NodeArea.cpp
	Ui/NodeAreaItems.h
	Ui/NodeItem.h
	Ui/NodeItem.cpp
	Ui/SlotItem.h
	Ui/LinkItem.h
	Ui/LinkItem.cpp
	Ui/UiApp.h
	Ui/UiApp.cpp
	Ui/UiWindow.h
	Ui/UiWindow.cpp
	Ui/ParameterDelegate.h
	Ui/ParameterDelegate.cpp
	Ui/QuadTree.h
	Ui/QuadTree.cpp
	Ui/NodeDelegate.h
	Ui/NodeDelegate.cpp

	main.cpp
)

add_executable(
	Gogh
	${SRC}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Gogh glad)
target_link_libraries(Gogh glfw)
target_link_libraries(Gogh nanovg)
target_link_libraries(Gogh tiny-process-library)

# For IDE projects
group_source_by_folder(${SRC})
