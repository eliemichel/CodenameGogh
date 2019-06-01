import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQml.Models 2.12

import "NodeDelegate/"

Rectangle {
    id: node

    property int modelIndex//: model.index
    property var outputs
    property var inputs
    property var parameters
    property string name
    property int delegateIndex // Must be set to DelegateModel.itemsIndex in caller view

    property bool movedBit // Hack to detect in slots that the node has moved

    color: "#272822"
    border.width: 1
    border.color: modelIndex == nodeSelectionModel.currentIndex.row ? "blue" : "black"
    width: 200
    height: 100

    onXChanged: { movedBit = !movedBit; }
    onYChanged: { movedBit = !movedBit; }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        NodeHeader {
            onPressed: {
                // Move node to foreground
                nodeDelegateModel.items.move(delegateIndex, nodeDelegateModel.items.count - 1)
            }
        }

        // Reminder of the node, for i/o and params
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Parameters
            Item {
                anchors.fill: parent

                ListView {
                    anchors.fill: parent
                    model: parameters
                    delegate: Text {
                        text: "- " + name
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        nodeSelectionModel.setCurrentIndex(nodeGraphModel.nodes.index(node.modelIndex, 0), ItemSelectionModel.Current)
                    }
                }

            }

            // Inputs
            Column {
                spacing: 10
                anchors.left: parent.left
                Repeater {
                    model: inputs

                    Slot {
                        slotType: "input"
                    }
                }
            }

            // Outputs
            Column {
                spacing: 10
                anchors.right: parent.right
                Repeater {
                    model: outputs

                    Slot {
                        slotType: "output"
                    }
                }
            }
        }
    }
}
