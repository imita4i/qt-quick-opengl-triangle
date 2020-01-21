import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Window 2.11

import test_figure 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Triangle")

    GLESFigure {
        anchors.fill: parent
        focus: true
    }
}
