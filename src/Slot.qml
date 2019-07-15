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
      //Node node = parent.parent.parent.nodes[0]
      //console.log(nodes[0].output);
      // slot snapping
      var slots = [];
      for (var i=0; i < nodes.length; i++){
        if (type == 0){
          slots.push(nodes[i].output);
        } else {
          slots.push(nodes[i].input);
        }
      }

      for (var i=0; i < slots.length; i++){
        var dx = (slots[i].parent.x + slots[i].x) - (draggerPlug.parent.parent.x + draggerPlug.parent.x + draggerPlug.x);
        var dy = (slots[i].parent.y + slots[i].y) - (draggerPlug.parent.parent.y + draggerPlug.parent.y + draggerPlug.y);
        var distance = Math.sqrt(dx*dx + dy*dy);

        if (distance < 25){
          if(type == 0){
          target = slots[i].plug;
        } else {
          slots[i].target = draggerPlug.parent.plug;
        }
        }
        console.log("target", slots[i], target);
      }

      // Reset
      draggerPlug.x = plug.x;
      draggerPlug.y = plug.y;
      dragger = false;
    }
  }
}
