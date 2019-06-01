import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQml.Models 2.12
import QtQuick.Shapes 1.12

Rectangle {
    id: nodeGraphView
    color: "#dddddd"

    Component {
        id: nodeDelegate

        Rectangle {
            id: node
            property int modelIndex: index
            color: "#272822"
            border.width: 1
            border.color: index == nodeSelectionModel.currentIndex.row ? "blue" : "black"
            width: 200
            height: 100

            // Hack to detect in slots that the node has moved
            property bool movedBit
            onXChanged: { movedBit = !movedBit; }
            onYChanged: { movedBit = !movedBit; }

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // Header of the node, with the node name and that is used for dragging
                Rectangle {
                    id: nodeDragHandle
                    color: "#472822"
                    Layout.fillWidth: true
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

                            Rectangle {
                                width: 15
                                height: 15
                                color: "red";
                                Text {
                                    text: name
                                    anchors.fill: parent
                                    color: "white"
                                }
                                Binding {
                                    target : model
                                    property : "x"
                                    value : {
                                        node.movedBit = node.movedBit; // force dependency
                                        mapToItem(nodeGraphViewRoot, width/2, height/2).x
                                    }
                                }
                                Binding {
                                    target : model
                                    property : "y"
                                    value : {
                                        node.movedBit = node.movedBit; // force dependency
                                        mapToItem(nodeGraphViewRoot, width/2, height/2).y
                                    }
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onPressed: {
                                        console.log("pressed input #" + index + " of node #" + node.modelIndex);
                                        nodeGraphModel.pendingEdges.insert(nodeGraphModel.pendingEdges.count, {
                                            originNode: -1,
                                            originOutput: -1,
                                            destinationNode: node.modelIndex,
                                            destinationInput: index,
                                        })
                                    }
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
                                Binding {
                                    target : model
                                    property : "x"
                                    value : {
                                        node.movedBit = node.movedBit; // force dependency
                                        mapToItem(nodeGraphViewRoot, width/2, height/2).x
                                    }
                                }
                                Binding {
                                    target : model
                                    property : "y"
                                    value : {
                                        node.movedBit = node.movedBit; // force dependency
                                        mapToItem(nodeGraphViewRoot, width/2, height/2).y
                                    }
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onPressed: {
                                        console.log("pressed output #" + index + " of node #" + node.modelIndex);
                                        nodeGraphModel.pendingEdges.insert(nodeGraphModel.pendingEdges.count, {
                                            originNode: node.modelIndex,
                                            originOutput: index,
                                            destinationNode: -1,
                                            destinationInput: -1,
                                        })
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: edgeDelegate

        Shape {
            function getOutputPosition(nodeIndex, outputIndex) {
                if (nodeIndex == -1) {
                    return Qt.point(globalMousePosition.mouseX, globalMousePosition.mouseY);
                } else {
                    return nodeGraphModel.nodes.get(nodeIndex).outputs.get(outputIndex);
                }
            }
            function getInputPosition(nodeIndex, inputIndex) {
                if (nodeIndex == -1) {
                    return Qt.point(globalMousePosition.mouseX, globalMousePosition.mouseY);
                } else {
                    return nodeGraphModel.nodes.get(nodeIndex).inputs.get(inputIndex);
                }
            }

            anchors.fill: parent
            ShapePath {
                strokeWidth: 2
                strokeColor: "black"
                startX: getOutputPosition(originNode, originOutput).x
                startY: getOutputPosition(originNode, originOutput).y
                PathLine {
                    x: getInputPosition(destinationNode, destinationInput).x
                    y: getInputPosition(destinationNode, destinationInput).y
                }
            }
        }
    }

    Item {
        id: nodeGraphViewRoot
        anchors.fill: parent

        DelegateModel {
            id: nodeDelegateModel
            model: nodeGraphModel.nodes
            delegate: nodeDelegate
        }

        DelegateModel {
            id: edgeDelegateModel
            model: nodeGraphModel.edges
            delegate: edgeDelegate
        }

        DelegateModel {
            id: pendingEdgeDelegateModel
            model: nodeGraphModel.pendingEdges
            delegate: edgeDelegate
        }

        Repeater { model: nodeDelegateModel }
        Item {
            id: edgesLayer // Layer for antialiasing
            anchors.fill: parent
            layer.enabled: true
            layer.samples: 4
            Repeater { model: edgeDelegateModel }
            Repeater { model: pendingEdgeDelegateModel }
        }

        MouseArea { // TODO: this design causes a problem with cursorShape
            id: globalMousePosition
            anchors.fill: parent
            hoverEnabled: true
            propagateComposedEvents: true
            onPressed: { mouse.accepted = false }
        }
    }
}
