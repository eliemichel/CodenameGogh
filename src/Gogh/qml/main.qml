import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0
import QtQml.Models 2.12

ApplicationWindow
{
    visible: true
    width: 640
    height: 480
    title: qsTr("Gogh - DEVELOPMENT VERSION")

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

    StackView {
        id: stack
        anchors.fill: parent
    }

    RowLayout {
        id: rowLayout
        anchors.fill: parent
        spacing: 0

        property real sliderPercentage: 50
        property real sliderPixels: Math.min(Math.max(20, sliderPercentage/100.0*width), width - 20)

        NodeView{
            id: nodeView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: rowLayout.sliderPixels
            Layout.maximumWidth: rowLayout.sliderPixels
            Layout.preferredWidth: rowLayout.sliderPixels
        }

        Rectangle {
            id: slider
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: 4
            Layout.maximumWidth: 4
            color: "red"
            MouseArea {
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

        Rectangle {
            color: 'plum'
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: 20
            Text {
                anchors.centerIn: parent
                text: rowLayout.sliderPercentage + 'x' + rowLayout.sliderPixels
            }
        }

    }

}
