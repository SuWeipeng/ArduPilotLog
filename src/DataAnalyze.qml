import QtQuick                  2.5
import QtQuick.Controls         1.2
import QtQuick.Controls.Styles  1.2
import QtQuick.Dialogs          1.2
import QtQuick.Window           2.2

import ArduPilotLog.Controllers   1.0

Rectangle {
    property real _margins: 5
    property real _Width  : 47
    property real _Height : 20

    property bool button_state_1: false // true: hide

    id: dataAnalyze
    width: 400
    height: 300

    DataAnalyzeController {
        id:         controller
    }

    AutoResize{
       id:globalResize
    }

    Label{
        anchors.top       : parent.top
        anchors.topMargin : _margins
        anchors.left      : parent.left
        anchors.leftMargin: _margins

        id: _labelState
        width : _Width
        height: _Height

        text: qsTr("State")
    }
    Label{
        anchors.top       : _labelState.top
        anchors.left      : _labelState.right
        anchors.leftMargin: _margins

        id: _labelTable
        width : _Width
        height: _Height

        text: qsTr("Table")
    }
    Label{
        anchors.top       : _labelTable.top
        anchors.left      : _labelTable.right
        anchors.leftMargin: _margins

        id: _labelField
        width : _Width
        height: _Height

        text: qsTr("Field")
    }
    Label{
        anchors.top       : _labelField.top
        anchors.left      : _labelField.right
        anchors.leftMargin: _margins

        id: _labelScale
        width : _Width
        height: _Height

        text: qsTr("Scale")
    }
    Label{
        anchors.top       : _labelScale.top
        anchors.left      : _labelScale.right
        anchors.leftMargin: _margins

        id: _labelOffsetX
        width : _Width
        height: _Height

        text: qsTr("X+")
    }
    Label{
        anchors.top       : _labelOffsetX.top
        anchors.left      : _labelOffsetX.right
        anchors.leftMargin: _margins

        id: _labelOffsetY
        width : _Width
        height: _Height

        text: qsTr("Y+")
    }
    Label{
        anchors.top       : _labelOffsetY.top
        anchors.left      : _labelOffsetY.right
        anchors.leftMargin: _margins

        id: _labelLine
        width : _Width
        height: _Height

        text: qsTr("Line")
    }
    Label{
        anchors.top       : _labelLine.top
        anchors.left      : _labelLine.right
        anchors.leftMargin: _margins

        id: _labelColor
        width : _Width
        height: _Height

        text: qsTr("Color")
    }

    // Row 1
    Button{
        anchors.top       : _labelState.bottom
        anchors.topMargin : _margins
        anchors.left      : _labelState.left

        id: _buttonState1
        width : _Width
        height: _Height
        text  : {
            if(controller.visible1){
                qsTr("1")
            }else{
                qsTr("hide")
            }
        }
        onClicked:{
            controller.setVisible1(!controller.visible1)
        }
    }
    ComboBox{
        anchors.top       : _buttonState1.top
        anchors.left      : _buttonState1.right
        anchors.leftMargin: _margins

        id    : _comboboxTable1
        width : _Width
        height: _Height
        model : controller.tableList
        onCurrentTextChanged: {
            controller.setFieldList1(_comboboxTable1.currentText)
        }
    }
    ComboBox{
        anchors.top       : _comboboxTable1.top
        anchors.left      : _comboboxTable1.right
        anchors.leftMargin: _margins

        id    : _comboboxField1
        width : _Width
        height: _Height
        model : controller.fieldList1
        onCurrentTextChanged: {
            controller.setField1(_comboboxField1.currentText)
        }
    }
    TextField{
        anchors.top       : _comboboxField1.top
        anchors.left      : _comboboxField1.right
        anchors.leftMargin: _margins

        id   : _textfieldScale1
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.scale1
        onAccepted: {
            controller.setScale1(_textfieldScale1.text)
        }
    }
    TextField{
        anchors.top       : _textfieldScale1.top
        anchors.left      : _textfieldScale1.right
        anchors.leftMargin: _margins

        id   : _textfieldX1
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetX1
        onAccepted: {
            controller.setOffsetX1(_textfieldX1.text)
        }
    }
    TextField{
        anchors.top       : _textfieldX1.top
        anchors.left      : _textfieldX1.right
        anchors.leftMargin: _margins

        id   : _textfieldY1
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetY1
        onAccepted: {
            controller.setOffsetY1(_textfieldY1.text)
        }
    }
    ComboBox{
        anchors.top       : _textfieldY1.top
        anchors.left      : _textfieldY1.right
        anchors.leftMargin: _margins

        id    : _comboboxLine1
        width : _Width
        height: _Height
        model : controller.lineList
        onCurrentTextChanged: {
            controller.setLineStyle1(parseInt(_comboboxLine1.currentIndex))
        }
    }
    ComboBox{
        anchors.top       : _comboboxLine1.top
        anchors.left      : _comboboxLine1.right
        anchors.leftMargin: _margins

        id    : _comboboxColor1
        width : _Width
        height: _Height
        model: controller.colorList
        onCurrentTextChanged: {
            controller.setLineColor1(parseInt(_comboboxColor1.currentIndex))
        }
    }
}
