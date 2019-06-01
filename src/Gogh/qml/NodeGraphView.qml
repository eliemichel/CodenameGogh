import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQml.Models 2.12
import QtQuick.Shapes 1.12

Rectangle {
    id: nodeGraphView
    color: "#dddddd"

    // I did not find a way to avoid manually binding all the model item
    // properties one by one with lines such as "outputs: model.outputs"
    Component {
        id: nodeDelegate
        NodeDelegate {
            delegateIndex: DelegateModel.itemsIndex
            modelIndex: index
            outputs: model.outputs
            inputs: model.inputs
            parameters: model.parameters
            name: model.name
        }
    }

    Component {
        id: edgeDelegate
        EdgeDelegate {
            originNode: model.originNode
            originOutput: model.originOutput
            destinationNode: model.destinationNode
            destinationInput: model.destinationInput
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

        // Invisible object dragged when creating new links
        // It is either the origin of the destination of pending edges
        // i.e. edges that are being drawn
        Rectangle {
            id: pendingEdgeEnd

            property string typeKey: "output"
            property int targetNode: -1
            property int targetSlot: -1

            Drag.keys: [ typeKey ]
            Drag.active: true
        }
    }
}
