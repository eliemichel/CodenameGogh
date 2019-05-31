import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0

ApplicationWindow
{
    visible: true
    width: 640
    height: 480
    title: qsTr("Gogh - DEVELOPMENT VERSION")

    menuBar: ToolBar {
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: qsTr("‹")
                onClicked: stack.pop()
            }
            Label {
                text: "Title"
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
            ToolButton {
                text: qsTr("⋮")
                onClicked: menu.open()
            }
        }
    }

    StackView {
        id: stack
        anchors.fill: parent
    }

    Item {
        id: root
        anchors.fill: parent

        Repeater {
            model: 3

            Rectangle {
                id: node
                color: "#272822"
                width: 200
                height: 100


                Rectangle {
                    id: nodeDragHandle
                    color: "#472822"
                    width: 200
                    height: 25

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.OpenHandCursor

                        drag.target: node
                        drag.axis: Drag.XAndYAxis

                        onPressed: {
                            Qt.ClosedHandCursor
                        }
                        onReleased: {
                            Qt.OpenHandCursor
                        }
                    }

                    Text {
                        text: "Input Node"
                        anchors.fill: parent
                        leftPadding: 5
                        rightPadding: 5
                        verticalAlignment: Text.AlignVCenter
                        color: "white"
                    }
                }
            }
        }
    }
}