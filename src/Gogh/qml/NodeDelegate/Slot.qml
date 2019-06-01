import QtQuick 2.12

Rectangle {
    id: thisSlot
    property string slotType: "input" // must be either "input" or "output"

    width: 15
    height: 15
    color: "red";
    Text {
        text: name
        anchors.fill: parent
        horizontalAlignment: slotType == "input" ? Text.AlignLeft : Text.AlignRight
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
        drag.target: pendingEdgeEnd
        drag.smoothed: false

        function makeEdge(nodeA, slotA, nodeB, slotB, fromAToB) {
            return {
                originNode: fromAToB ? nodeA : nodeB,
                originOutput: fromAToB ? slotA : slotB,
                destinationNode: fromAToB ? nodeB : nodeA,
                destinationInput: fromAToB ? slotB : slotA,
            }
        }

        function addPendingEdges() {
            for (var i = 0, n = nodeGraphModel.pendingEdges.count ; i < n ; ++i) {
                var edge = nodeGraphModel.pendingEdges.get(i);
                
                nodeGraphModel.edges.append(
                    thisSlot.slotType == "input"
                    ? {
                        originNode: pendingEdgeEnd.targetNode,
                        originOutput: pendingEdgeEnd.targetSlot,
                        destinationNode: edge.destinationNode,
                        destinationInput: edge.destinationInput,
                    }
                    : {
                        originNode: edge.originNode,
                        originOutput: edge.originOutput,
                        destinationNode: pendingEdgeEnd.targetNode,
                        destinationInput: pendingEdgeEnd.targetSlot,
                    }
                );
            }
        }

        onPressed: {
            pendingEdgeEnd.x = model.x
            pendingEdgeEnd.y = model.y
            pendingEdgeEnd.typeKey = thisSlot.slotType == "input" ? "output" : "input";
            nodeGraphModel.pendingEdges.append(makeEdge(
                -1, -1,
                node.modelIndex, index,
                thisSlot.slotType == "input"
            ));
        }
        onReleased: {
            if (pendingEdgeEnd.targetNode != -1
                && pendingEdgeEnd.targetNode != node.modelIndex) // TODO: check for cycles
            {
                addPendingEdges();
            }
            pendingEdgeEnd.targetNode = -1
            pendingEdgeEnd.targetSlot = -1
            nodeGraphModel.pendingEdges.clear()
        }
    }
    DropArea {
        anchors.fill: parent
        keys: [ thisSlot.slotType ]
        onEntered: {
            thisSlot.color = "green"
            pendingEdgeEnd.targetNode = node.modelIndex
            pendingEdgeEnd.targetSlot = index
        }
        onExited: {
            thisSlot.color = "red"
            pendingEdgeEnd.targetNode = -1
            pendingEdgeEnd.targetSlot = -1
        }
    }
}
