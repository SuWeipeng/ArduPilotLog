import QtQuick                  2.5
import QtQuick.Controls         2.2
import QtQuick.Controls.Styles  1.2
import QtQuick.Dialogs          1.2
import QtQuick.Window           2.2

import ArduPilotLog.Controllers   1.0

Rectangle {
    property real _margins: 10
    property real _Width  : 50
    property real _Height : 30

    property bool button1_state: false

    id: dataAnalyze
    width: 400
    height: 300

    AutoResize{
       id:globalResize
    }

    Button{
        anchors.top       : parent.top
        anchors.topMargin : _margins
        anchors.left      : parent.left
        anchors.leftMargin: _margins

        id: _button1
        width : _Width
        height: _Height
        text:{
            if(!button1_state)
                qsTr("1")
            else
                qsTr("hide")
        }
        onClicked: {
            button1_state = !button1_state
        }
    }
}
