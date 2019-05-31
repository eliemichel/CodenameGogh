import QtQuick 2.0

Rectangle {
	id: verticalHandleDelegate
	height: parent.height
	width: 2
	color: styleData.pressed ? "#505050" : (styleData.hovered ? "#2a2a2a" : "#222222")

	Item {
		y: 0.5*(parent.height-height)
		anchors.leftMargin: -1
		Column {
			spacing: 2
			Repeater {
				model: 3
				Rectangle {
					width: 2
					height: 2
					color: "#666666"
				}
			}
		}
	}
}
