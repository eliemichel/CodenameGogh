import QtQuick 2.3
import QtQuick.Controls 1.0
import QtQuick.Window 2.3
import QtQuick.Shapes 1.0

ApplicationWindow
{
    visible: true
    width: 640
    height: 480
    title: qsTr("Gogh QML Proto")
    id: appWindow


    Rectangle {
    id: simpleButton
    width: 150; height: 75
    color: "grey"

    Text {
        id: buttonLabel
        anchors.centerIn: parent
        text: "button label"
    }

    MouseArea {
        id: buttonMouseArea

        // Anchor all sides of the mouse area to the rectangle's anchors
        anchors.fill: parent
        // onClicked handles valid mouse button clicks
        onClicked: console.log("length: " + nodes[0].label)
    }
  }
    Node {
      id: patateNode
      color: "blue"
      label: "Patate Node"
      x: 150; y: 150
      input.target: plopNode.output.plug //TODO: This Slot->Plug design is not very satisfying
  }

  Node {
    id: plopNode
    color: "purple"
    label: "Plop Node"
    x: 200; y: 200
}

  Link{
    id: link
  }

  property list<Node> nodes
  nodes: [ patateNode, plopNode ]
  property bool dragger

  property Node node: nodes[0]
  /*property Shape shape: Shape {
    width: 20
    ShapePath {
        strokeColor: "blue"
        strokeWidth: 2
        strokeStyle: ShapePath.DashLine
        startX: nodes[index].x + nodes[index].input.plug.x
        startY: nodes[index].y + nodes[index].input.plug.y
        PathLine { x: nodes[index].input.target.parent.parent.x + nodes[index].input.target.x
                   y: nodes[index].input.target.parent.parent.y + nodes[index].input.target.y }
    }
  }
  /*property Shape shape: Shape {
    width: 20
    ShapePath {
        strokeColor: "blue"
        strokeWidth: 2
        strokeStyle: ShapePath.DashLine
        startX: node.x + node.input.plug.x
        startY: node.y + node.input.plug.y
        PathLine { x: node.input.target.parent.parent.x + node.input.target.x
                   y: node.input.target.parent.parent.y + node.input.target.y }
    }
  }*/

  Repeater {
    model: nodes.length
    Shape {
      visible: nodes[index].input.target==null?false:true
      width: 20
      ShapePath {
          strokeColor: "blue"
          strokeWidth: 2
          strokeStyle: ShapePath.DashLine
          startX: nodes[index].x + nodes[index].input.x
          startY: nodes[index].y + nodes[index].input.y
          PathLine { x: visible?nodes[index].input.target.parent.parent.x + nodes[index].input.target.parent.x:0
                     y: visible?nodes[index].input.target.parent.parent.y + nodes[index].input.target.parent.y:0 }//condition made for avoiding null error
        }
      }
    }

    property Node draggerNode: nodes[0]
    property Slot draggedSlot: draggerNode.input

    Shape {
      id: draggerLink
      visible: dragger
      width: 20


      ShapePath {
          strokeColor: "yellow"
          strokeWidth: 2
          strokeStyle: ShapePath.DashLine
          startX: draggerNode.x + draggedSlot.x + draggedSlot.draggerPlug.width/2
          startY: draggerNode.y + draggedSlot.y + draggedSlot.draggerPlug.height/2
          PathLine { x: visible?draggerNode.x + draggedSlot.x + draggedSlot.draggerPlug.x + draggedSlot.draggerPlug.width/2:0
                     y: visible?draggerNode.y + draggedSlot.y + draggedSlot.draggerPlug.y + draggedSlot.draggerPlug.height/2:0 }//condition made for avoiding null error
        }
      }

  /*Component.onCompleted: {
    //for (int i=0; i < nodes.length; i++){
        var menu = Qt.createQmlObject('import QtQuick.Controls 2.0 ; Shape {
          width: 20
          ShapePath {
              strokeColor: "blue"
              strokeWidth: 2
              strokeStyle: ShapePath.DashLine
              startX: nodes[0].x + nodes[0].input.plug.x
              startY: nodes[0].y + nodes[0].input.plug.y
              PathLine { x: nodes[0].input.target.parent.parent.x + nodes[0].input.target.x
                         y: nodes[0].input.target.parent.parent.y + nodes[0].input.target.y }
        }}', appWindow,"dynamicSnippet1");
    //}
  }*/


}
