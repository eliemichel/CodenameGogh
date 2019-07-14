import QtQuick 2.3

Item {
  property alias head: head
  property alias tail: tail
  /*Shape {
    width: 20
    ShapePath {
        strokeColor: "blue"
        strokeWidth: 2
        strokeStyle: ShapePath.DashLine
        startX: 0
        startY: 0
        PathLine { x: input.x; y: input.y}
    }
  }*/

  Item {
    // Head
     id: head
     x: 100; y: 100
     width: 10; height: 10

     Rectangle {
          x: -width/2; y: -height/2
          width: 10
          height: width
          color: "green"
          border.color: "black"
          border.width: 1
          radius: width*0.5
        }

     Text {
          anchors.centerIn: parent
          color: "green"
          text: "Head"
     }

     Drag.active: dragAreaHead.drag.active
     Drag.hotSpot.x: 5
     Drag.hotSpot.y: 5

     MouseArea {
         id: dragAreaHead
         anchors.fill: parent

         drag.target: parent
         onPositionChanged: console.log("patate")
     }


  }

  Item {
    // Tail
     id: tail
     x: 10; y: 10
     width: 10; height: 10

     Rectangle {
          x: -width/2; y: -height/2
          width: 10
          height: width
          color: "yellow"
          border.color: "black"
          border.width: 1
          radius: width*0.5
        }

     Text {
          anchors.centerIn: parent
          color: "yellow"
          text: "Tail"
     }

     Drag.active: dragAreaTail.drag.active
     Drag.hotSpot.x: 5
     Drag.hotSpot.y: 5

     MouseArea {
         id: dragAreaTail
         anchors.fill: parent

         drag.target: parent
     }
  }

}
