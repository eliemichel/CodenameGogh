# This file is part of Augen Light
#
# Copyright (c) 2017 - 2019 -- Élie Michel <elie.michel@exppad.com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the “Software”), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# The Software is provided “as is”, without warranty of any kind, express or
# implied, including but not limited to the warranties of merchantability,
# fitness for a particular purpose and non-infringement. In no event shall the
# authors or copyright holders be liable for any claim, damages or other
# liability, whether in an action of contract, tort or otherwise, arising
# from, out of or in connection with the software or the use or other dealings
# in the Software.

# Automatically generate moc files for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED CONFIG)
if(Qt5Widgets_FOUND)
	message("Using Qt ${Qt5Widgets_VERSION} from ${Qt5Core_DIR}")
endif()

set(SRC
	FileProbeProcess.h
	FileProbeProcess.cpp
	GoghFlowScene.h
	GoghFlowScene.cpp
	ImageSequence.h
	ImageSequence.cpp
	MainWindow.h
	MainWindow.cpp
	RenderCommand.h
	RenderCommand.cpp

	nodes/AbstractStreamData.h
	nodes/VideoStreamData.h
	nodes/AudioStreamData.h
	nodes/SubtitleStreamData.h
	nodes/DataStreamData.h

	nodes/AbstractCodecData.h
	nodes/VideoCodecData.h
	nodes/AudioCodecData.h
	nodes/SubtitleCodecData.h
	nodes/DataCodecData.h

	nodes/FileInputDataModel.h
	nodes/FileInputDataModel.cpp
	nodes/FileOutputDataModel.h
	nodes/FileOutputDataModel.cpp

	nodes/H264CodecDataModel.h
	nodes/H264CodecDataModel.cpp

	nodes/AacCodecDataModel.h
	nodes/AacCodecDataModel.cpp

	widgets/FileInputWidget.h
	widgets/FileInputWidget.cpp
	widgets/GoghFlowView.h
	widgets/GoghFlowView.cpp

	dialogs/RenderDialog.h
	dialogs/RenderDialog.cpp

	main.cpp
)

add_executable (QNodeEditorTest ${SRC})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries_and_dll(QNodeEditorTest NodeEditor::nodes)
target_link_libraries_and_dll(QNodeEditorTest Qt5::Core)
target_link_libraries_and_dll(QNodeEditorTest Qt5::Gui)
target_link_libraries_and_dll(QNodeEditorTest Qt5::Widgets)

# For IDE projects
group_source_by_folder(${SRC})
