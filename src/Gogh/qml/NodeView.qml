import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQml.Models 2.12

Rectangle {
    id: nodeView
    color: "#dddddd"

    Component {
        id: nodeDelegate

        Rectangle {
            id: node
            color: "#272822"
            border.width: 1
            border.color: index == nodeSelectionModel.currentIndex.row ? "blue" : "black"
            width: 200
            height: 100

            MouseArea {
                anchors.fill: parent
                onClicked: nodeSelectionModel.setCurrentIndex(nodeModel.index(index, 0), ItemSelectionModel.Current)
            }

            ColumnLayout {
                // Header of the node, with the node name and that is used for dragging
                Rectangle {
                    id: nodeDragHandle
                    color: "#472822"
                    width: 200
                    height: 25

                    MouseArea {
                        property bool held: false

                        anchors.fill: parent
                        cursorShape: held ? Qt.ClosedHandCursor : Qt.OpenHandCursor

                        drag.target: node
                        drag.axis: Drag.XAndYAxis
                        drag.smoothed: false

                        onPressed: {
                            held = true
                            nodeDelegateModel.items.move(node.DelegateModel.itemsIndex, nodeDelegateModel.items.count - 1)
                        }
                        onReleased: held = false
                    }

                    Text {
                        text: name
                        anchors.fill: parent
                        leftPadding: 5
                        rightPadding: 5
                        verticalAlignment: Text.AlignVCenter
                        color: "white"
                    }
                }

                // Reminder of the node, for i/o and params
                Rectangle {
                    color: "green"
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    // Inputs
                    Column {
                        spacing: 10
                        anchors.left: parent.left
                        Repeater {
                            model: inputs

                            Rectangle {
                                width: 15
                                height: 15
                                color: "red";
                                Text {
                                    text: name
                                    anchors.fill: parent
                                    color: "white"
                                }
                            }
                        }
                    }

                    // Outputs
                    Column {
                        spacing: 10
                        anchors.right: parent.right
                        Repeater {
                            model: outputs

                            Rectangle {
                                width: 15
                                height: 15
                                color: "red";
                                Text {
                                    text: name
                                    anchors.right: parent.right
                                    color: "white"
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Item {
        id: root
        anchors.fill: parent

        DelegateModel {
            id: nodeDelegateModel
            model: nodeModel
            delegate: nodeDelegate
        }

        Repeater {
            model: nodeDelegateModel
        }
    }
}
