import QtQuick 2.12
import QtQuick.Layouts 1.0
import QtQml.Models 2.12
import QtQuick.Controls 1.5 as QQC1

Rectangle {
	id: parameterListView
	property var model: nodeGraphModel.nodes.get(nodeSelectionModel.currentIndex.row)

	ColumnLayout {
		anchors.fill: parent

		ListView {
			Layout.fillWidth: true;
	        model: parameterListView.model != undefined ? parameterListView.model.parameters : 0
	        delegate: Text {
	        	color: "black"
	            text: "- " + name
	        }
	    }

	    QQC1.TreeView { // DEBUG
	    	model: nodeGraphModel
	    	Layout.fillWidth: true;
	    	Layout.fillHeight: true;
	    }
	}
}
