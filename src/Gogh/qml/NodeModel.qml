import QtQuick 2.0

ListModel {
    id: nodeModel

    ListElement {
        name: "Input Node"
        inputs: []
        outputs: [
            ListElement {
                name: "Video"
            },
            ListElement {
                name: "Audio"
            }
        ]
    }
    ListElement {
        name: "Encode Node"
        inputs: [
            ListElement {
                name: "Video"
            }
        ]
        outputs: [
            ListElement {
                name: "Video"
            }
        ]
    }
    ListElement {
        name: "Output Node"
        inputs: [
            ListElement {
                name: "Video"
            },
            ListElement {
                name: "Audio"
            }
        ]
        outputs: []
    }
}
