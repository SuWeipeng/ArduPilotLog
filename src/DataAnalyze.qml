import QtQuick                  2.5
import QtQuick.Controls         1.2
import QtQuick.Controls.Styles  1.2
import QtQuick.Dialogs          1.2
import QtQuick.Window           2.2

import ArduPilotLog.Controllers   1.0

GroupBox {
    anchors.margins: 50

    id: _groupbox
    title: qsTr("Hello")

    Label{
        anchors.top       : _groupbox.top
        anchors.left      : _groupbox.right
        anchors.leftMargin: 50

        id    : _label
        width : 10
        height: 10
        text  : "world"
    }
}



