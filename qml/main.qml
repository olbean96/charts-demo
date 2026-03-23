import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

Window {
    id: root
    width: 1180
    height: 720
    visible: true
    title: "QImage/QPainter Charts Demo"
    color: "#f4efe7"

    ScrollView {
        id: scrollView
        anchors.fill: parent
        clip: true

        ChartsWidget {
            manager: graphManager
            fillAvailableWhenSmaller: true
            availableWidth: scrollView.availableWidth
            availableHeight: scrollView.availableHeight
        }
    }
}
