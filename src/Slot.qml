import QtQuick 2.3

Item {
  enum SlotType {
    Input,
    Output
  }

  id: slot
  property int type
  property alias plug: plug
  property alias draggerPlug: draggerPlug
  property Plug target

  x: switch(type){
  case 0: -width/2
  break
  case 1: parent.width-width/2
  break
  }

  y: parent.height/2
  width: 10
  height: width

  Plug {
    id: plug
    plugColor: "white"
  }

  Plug {
    id: draggerPlug
    plugColor: "red"
    visible: dragAreaPlug.drag.active
  }

  MouseArea {
    id: dragAreaPlug
    anchors.fill: parent

    drag.target: draggerPlug
    //drag.threshold: 5
    onPressed: {
      //console.log(draggedSlot);
      dragger = true;
      draggerNode = parent.parent;
      draggedSlot = parent;
      parent.target = null;
    }
    //onPositionChanged: drag.active?console.log("drag moved"):console.log("moved")
    onReleased: {
      draggerPlug.x = plug.x;
      draggerPlug.y = plug.y;
      dragger = false;

      //Node node = parent.parent.parent.nodes[0]
      console.log(nodes[0].output);
      // slot snapping
      var slots = [];
      for (var i=0; i < nodes.length; i++){
        if (type == 0){
          slots.push(nodes[i].output);
        } else {
          slots.push(nodes[i].input);
        }
      }
      console.log(slots);
    }
  }
}
