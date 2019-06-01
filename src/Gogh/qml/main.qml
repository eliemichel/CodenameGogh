import QtQuick 2.12
import QtQml.Models 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0

ApplicationWindow
{
    visible: true
    width: 900
    height: 600
    title: qsTr("Gogh - DEVELOPMENT VERSION")

    // <MODELS> //
    NodeGraphModel {
        id: nodeGraphModel
    }

    ItemSelectionModel {
        id: nodeSelectionModel
        model: nodeGraphModel.nodes
    }
    // </MODELS> //

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: console.log("Open action triggered");
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    // SplitView reimplemented (because available only from Qt 5.13 on and ATM I am using 5.12)
    RowLayout {
        id: rowLayout
        anchors.fill: parent
        spacing: 0

        property real sliderPercentage: 75
        property real sliderPixels: Math.min(Math.max(20, sliderPercentage/100.0*width), width - 20)

        NodeGraphView {
            id: nodeView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: rowLayout.sliderPixels
            Layout.maximumWidth: rowLayout.sliderPixels
            Layout.preferredWidth: rowLayout.sliderPixels
        }

        VerticalHandleDelegate {
            id: slider
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: 2
            Layout.maximumWidth: 2
            QtObject {
                id: styleData
                property bool pressed: mouseArea.held
                property bool hovered: false
            }
            MouseArea {
                id: mouseArea
                property bool held;
                property real deltaX;

                anchors.fill: parent
                cursorShape: Qt.SizeHorCursor
                onPressed: {
                    held = true;
                    deltaX = rowLayout.sliderPixels - mapToItem(rowLayout, mouseX, 0).x;
                }
                onPositionChanged: {
                    if (held) {
                        rowLayout.sliderPercentage = (mapToItem(rowLayout, mouseX, 0).x + deltaX) / rowLayout.width * 100.
                    }
                }
                onReleased: held = false
            }
        }

        ParameterListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: 20
        }

    }

}
