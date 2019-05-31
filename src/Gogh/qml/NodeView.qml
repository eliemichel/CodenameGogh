import QtQuick 2.12
import QtQml.Models 2.12

Rectangle {
    id: nodeView
    color: "#dddddd"

    Component {
        id: nodeDelegate

        Rectangle {
            id: node
            color: "#272822"
            width: 200
            height: 100

            Rectangle {
                // Header of the node, with the node name and that is used for dragging
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
                        nodeModel.items.move(node.DelegateModel.itemsIndex, nodeModel.items.count - 1)
                    }
                    onReleased: held = false
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

    Item {
        id: root
        anchors.fill: parent

        DelegateModel {
            id: nodeModel
            model: 3
            delegate: nodeDelegate
        }

        Repeater {
            model: nodeModel
        }
    }
}
