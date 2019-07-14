import QtQuick 2.3

// Node
Rectangle {
  id: node
  x: 10; y: 10
  width: 100; height: 50
  color: "red"

  property string label: "node"
  property alias input: input
  property alias output: output


  Text {
      id: nodeLabel
      anchors.centerIn: parent
      text: label
  }

  Drag.active: dragArea.drag.active
  Drag.hotSpot.x: 5
  Drag.hotSpot.y: 5

  MouseArea {
      id: dragArea
      anchors.fill: parent

      drag.target: parent
  }

  // Input
       //property Slot linked: Slot {id: slotInput; type: Slot.SlotType.Input}
   Slot {
     id: input
     type: Slot.SlotType.Input
   }

   // Output
        //property Slot linked: Slot {id: slotInput; type: Slot.SlotType.Input}
    Slot {
      id: output
      type: Slot.SlotType.Output
    }
}
