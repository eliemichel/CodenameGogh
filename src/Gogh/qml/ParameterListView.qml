import QtQuick 2.12

Rectangle {
	id: parameterListView
	property var model: nodeGraphModel.nodes.get(nodeSelectionModel.currentIndex.row)

	Text {
	    anchors.centerIn: parent
	    text: nodeGraphModel.nodes.get(1).inputs.get(0).x + "x" + nodeGraphModel.nodes.get(1).inputs.get(0).y
	}

	ListView {
		anchors.fill: parent
        model: parameterListView.model != undefined ? parameterListView.model.parameters : 0
        delegate: Text {
        	color: "black"
            text: "- " + name
        }
    }
}
