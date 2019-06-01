import QtQuick 2.0

ListModel {
    id: nodeModel

    ListElement {
        name: "Input Node"
        parameters: [
            ListElement {
                name: "file"
            }
        ]
        inputs: []
        outputs: [
            ListElement {
                name: "Video"
                // view related attributes -- to be moved in a proxy model
                x: 0
                y: 0
            },
            ListElement {
                name: "Audio"
                // view related attributes -- to be moved in a proxy model
                x: 0
                y: 0
            }
        ]

    }
    ListElement {
        name: "Encode Node"
        parameters: [
            ListElement {
                name: "preset"
            },
            ListElement {
                name: "crf"
            },
            ListElement {
                name: "pix_fmt"
            }
        ]
        inputs: [
            ListElement {
                name: "Video"
                // view related attributes -- to be moved in a proxy model
                x: 0
                y: 0
            }
        ]
        outputs: [
            ListElement {
                name: "Video"
                // view related attributes -- to be moved in a proxy model
                x: 0
                y: 0
            }
        ]
    }
    ListElement {
        name: "Output Node"
        parameters: [
            ListElement {
                name: "file"
            }
        ]
        inputs: [
            ListElement {
                name: "Video"
                // view related attributes -- to be moved in a proxy model
                x: 0
                y: 0
            },
            ListElement {
                name: "Audio"
                // view related attributes -- to be moved in a proxy model
                x: 0
                y: 0
            }
        ]
        outputs: []
    }
}
