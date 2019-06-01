import QtQuick 2.12
import QtQuick.Layouts 1.12

// Header of the node, with the node name and that is used for dragging
Rectangle {
    id: nodeHeader

    height: 25
    color: "#472822"
    Layout.fillWidth: true

    signal pressed

    MouseArea {
        property bool held: false

        anchors.fill: parent
        cursorShape: held ? Qt.ClosedHandCursor : Qt.OpenHandCursor

        drag.target: node
        drag.axis: Drag.XAndYAxis
        drag.smoothed: false

        onPressed: {
            held = true;
            nodeHeader.pressed();
        }
        onReleased: held = false
    }

    Text {
        id: nodeTitle
        
        text: name
        anchors.fill: parent
        leftPadding: 5
        rightPadding: 5
        verticalAlignment: Text.AlignVCenter
        color: "white"
    }
}
