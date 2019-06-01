import QtQuick 2.12
import QtQuick.Shapes 1.12

Shape {
    property int originNode
    property int originOutput
    property int destinationNode
    property int destinationInput

    function getOutputPosition(nodeIndex, outputIndex) {
        if (nodeIndex == -1) {
            return Qt.point(pendingEdgeEnd.x, pendingEdgeEnd.y);
        } else {
            return nodeGraphModel.nodes.get(nodeIndex).outputs.get(outputIndex);
        }
    }
    function getInputPosition(nodeIndex, inputIndex) {
        if (nodeIndex == -1) {
            return Qt.point(pendingEdgeEnd.x, pendingEdgeEnd.y);
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
