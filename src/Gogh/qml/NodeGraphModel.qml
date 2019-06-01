import QtQuick 2.0

Item {
    id: nodeGraphModel
    property alias nodes: nodeGraphModel_nodes
    property alias edges: nodeGraphModel_edges

    // NODES
    ListModel {
        id: nodeGraphModel_nodes

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

    // EDGES
    ListModel {
        id: nodeGraphModel_edges

        ListElement {
            originNode: 0
            originOutput: 0
            destinationNode: 1
            destinationInput: 0
        }

        ListElement {
            originNode: 1
            originOutput: 0
            destinationNode: 2
            destinationInput: 1
        }
    }
}
