import QtQuick 2.3

Item {
  x: 0; y: 0
  width: 10; height: width

  property string plugColor
  Rectangle {
    id: plugShape
    width: parent.width
    height: parent.height
    color: plugColor
    border.color: "black"
    border.width: 1
    radius: width*0.5

    Text {
      anchors.centerIn: parent
      color: plugColor
      text: switch(type){
      case 0: "Input"
      break
      case 1: "Output"
      break
    }
  }
  }
  //Item {
  /*Drag.active: dragAreaPlug.drag.active
  Drag.hotSpot.x: 5
  Drag.hotSpot.y: 5
  Drag.dragType: Drag.Internal
  Drag.onDragStarted: console.log("drag started")
  Drag.mimeData: {
            "text/plain": "Copied text"
        }

        Text {
                  id: text
                  anchors.centerIn: parent
                  text: "Drag me"
              }
*/
  //}
}
