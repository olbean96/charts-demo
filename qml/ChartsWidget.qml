import QtQuick 2.15
import QtQuick.Controls 2.15
import ChartsDemo 1.0

Item {
    id: root

    property var manager
    property bool fillAvailableWhenSmaller: false
    property real availableWidth: 0
    property real availableHeight: 0
    implicitWidth: manager ? manager.contentWidth + outerMargin * 2 : 960
    implicitHeight: manager ? manager.contentHeight + outerMargin * 2 : 640
    width: fillAvailableWhenSmaller ? Math.max(implicitWidth, availableWidth) : implicitWidth
    height: fillAvailableWhenSmaller ? Math.max(implicitHeight, availableHeight) : implicitHeight

    readonly property int outerMargin: 24
    readonly property int panelMargin: 28
    readonly property int panelPadding: 14
    readonly property int panelRadius: 10
    readonly property int rowSpacing: 10
    readonly property int legendLineWidth: 26
    readonly property int legendLineHeight: 4
    readonly property int toggleSize: 28

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#f9f4ec" }
            GradientStop { position: 1.0; color: "#ebe3d7" }
        }
    }

    GraphView {
        id: chartView
        anchors.fill: parent
        anchors.margins: root.outerMargin
        manager: root.manager
    }

    Column {
        id: legendContainer
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: root.panelMargin
        spacing: 8

        Rectangle {
            id: legendToggleButton
            width: root.toggleSize
            height: root.toggleSize
            anchors.right: parent.right
            radius: 6
            color: "#ffffff"
            border.color: "#d1c7ba"

            Text {
                anchors.centerIn: parent
                text: root.manager && root.manager.legendExpanded ? "▼" : "▶"
                color: "#2f2b28"
                font.pixelSize: 12
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (root.manager) {
                        root.manager.toggleLegendExpanded()
                    }
                }
            }
        }

        Rectangle {
            id: legendPanel
            anchors.right: parent.right
            visible: root.manager ? root.manager.legendExpanded : true
            radius: root.panelRadius
            color: "#ffffff"
            border.color: "#d1c7ba"
            implicitWidth: legendColumn.implicitWidth + root.panelPadding * 2
            implicitHeight: legendColumn.implicitHeight + root.panelPadding * 2

            Column {
                id: legendColumn
                anchors.fill: parent
                anchors.margins: root.panelPadding
                spacing: root.rowSpacing

                Text {
                    text: "Легенда"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#2f2b28"
                }

                Repeater {
                    model: root.manager ? root.manager.legendModel : null

                    delegate: Row {
                        spacing: 8

                        Rectangle {
                            width: root.legendLineWidth
                            height: root.legendLineHeight
                            radius: height / 2
                            color: model.color
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Text {
                            text: model.name
                            color: "#403830"
                            font.pixelSize: 14
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: root.manager

        function onXValueSelected(x) {
            console.log("Selected X:", x)
        }
    }
}
