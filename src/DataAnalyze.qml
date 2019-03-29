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
        model : controller.tableList1
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
        model: controller.colorList1
        onCurrentTextChanged: {
            controller.setLineColor1(_comboboxColor1.currentText)
        }
    }

    // Row 2
    Button{
        anchors.top       : _buttonState1.bottom
        anchors.topMargin : _margins
        anchors.left      : _buttonState1.left

        id: _buttonState2
        width : _Width
        height: _Height
        text  : {
            if(controller.visible2){
                qsTr("2")
            }else{
                qsTr("hide")
            }
        }
        onClicked:{
            controller.setVisible2(!controller.visible2)
        }
    }
    ComboBox{
        anchors.top       : _buttonState2.top
        anchors.left      : _buttonState2.right
        anchors.leftMargin: _margins

        id    : _comboboxTable2
        width : _Width
        height: _Height
        model : controller.tableList2
        onCurrentTextChanged: {
            controller.setFieldList2(_comboboxTable2.currentText)
        }
    }
    ComboBox{
        anchors.top       : _comboboxTable2.top
        anchors.left      : _comboboxTable2.right
        anchors.leftMargin: _margins

        id    : _comboboxField2
        width : _Width
        height: _Height
        model : controller.fieldList2
        onCurrentTextChanged: {
            controller.setField2(_comboboxField2.currentText)
        }
    }
    TextField{
        anchors.top       : _comboboxField2.top
        anchors.left      : _comboboxField2.right
        anchors.leftMargin: _margins

        id   : _textfieldScale2
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.scale2
        onAccepted: {
            controller.setScale2(_textfieldScale2.text)
        }
    }
    TextField{
        anchors.top       : _textfieldScale2.top
        anchors.left      : _textfieldScale2.right
        anchors.leftMargin: _margins

        id   : _textfieldX2
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetX2
        onAccepted: {
            controller.setOffsetX2(_textfieldX2.text)
        }
    }
    TextField{
        anchors.top       : _textfieldX2.top
        anchors.left      : _textfieldX2.right
        anchors.leftMargin: _margins

        id   : _textfieldY2
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetY2
        onAccepted: {
            controller.setOffsetY2(_textfieldY2.text)
        }
    }
    ComboBox{
        anchors.top       : _textfieldY2.top
        anchors.left      : _textfieldY2.right
        anchors.leftMargin: _margins

        id    : _comboboxLine2
        width : _Width
        height: _Height
        model : controller.lineList
        onCurrentTextChanged: {
            controller.setLineStyle2(parseInt(_comboboxLine2.currentIndex))
        }
    }
    ComboBox{
        anchors.top       : _comboboxLine2.top
        anchors.left      : _comboboxLine2.right
        anchors.leftMargin: _margins

        id    : _comboboxColor2
        width : _Width
        height: _Height
        model: controller.colorList2
        onCurrentTextChanged: {
            controller.setLineColor2(_comboboxColor2.currentText)
        }
    }

    // Row 3
    Button{
        anchors.top       : _buttonState2.bottom
        anchors.topMargin : _margins
        anchors.left      : _buttonState2.left

        id: _buttonState3
        width : _Width
        height: _Height
        text  : {
            if(controller.visible3){
                qsTr("3")
            }else{
                qsTr("hide")
            }
        }
        onClicked:{
            controller.setVisible3(!controller.visible3)
        }
    }
    ComboBox{
        anchors.top       : _buttonState3.top
        anchors.left      : _buttonState3.right
        anchors.leftMargin: _margins

        id    : _comboboxTable3
        width : _Width
        height: _Height
        model : controller.tableList3
        onCurrentTextChanged: {
            controller.setFieldList3(_comboboxTable3.currentText)
        }
    }
    ComboBox{
        anchors.top       : _comboboxTable3.top
        anchors.left      : _comboboxTable3.right
        anchors.leftMargin: _margins

        id    : _comboboxField3
        width : _Width
        height: _Height
        model : controller.fieldList3
        onCurrentTextChanged: {
            controller.setField3(_comboboxField3.currentText)
        }
    }
    TextField{
        anchors.top       : _comboboxField3.top
        anchors.left      : _comboboxField3.right
        anchors.leftMargin: _margins

        id   : _textfieldScale3
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.scale3
        onAccepted: {
            controller.setScale3(_textfieldScale3.text)
        }
    }
    TextField{
        anchors.top       : _textfieldScale3.top
        anchors.left      : _textfieldScale3.right
        anchors.leftMargin: _margins

        id   : _textfieldX3
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetX3
        onAccepted: {
            controller.setOffsetX3(_textfieldX3.text)
        }
    }
    TextField{
        anchors.top       : _textfieldX3.top
        anchors.left      : _textfieldX3.right
        anchors.leftMargin: _margins

        id   : _textfieldY3
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetY3
        onAccepted: {
            controller.setOffsetY3(_textfieldY3.text)
        }
    }
    ComboBox{
        anchors.top       : _textfieldY3.top
        anchors.left      : _textfieldY3.right
        anchors.leftMargin: _margins

        id    : _comboboxLine3
        width : _Width
        height: _Height
        model : controller.lineList
        onCurrentTextChanged: {
            controller.setLineStyle3(parseInt(_comboboxLine3.currentIndex))
        }
    }
    ComboBox{
        anchors.top       : _comboboxLine3.top
        anchors.left      : _comboboxLine3.right
        anchors.leftMargin: _margins

        id    : _comboboxColor3
        width : _Width
        height: _Height
        model: controller.colorList3
        onCurrentTextChanged: {
            controller.setLineColor3(_comboboxColor3.currentText)
        }
    }

    // Row 4
    Button{
        anchors.top       : _buttonState3.bottom
        anchors.topMargin : _margins
        anchors.left      : _buttonState3.left

        id: _buttonState4
        width : _Width
        height: _Height
        text  : {
            if(controller.visible4){
                qsTr("4")
            }else{
                qsTr("hide")
            }
        }
        onClicked:{
            controller.setVisible4(!controller.visible4)
        }
    }
    ComboBox{
        anchors.top       : _buttonState4.top
        anchors.left      : _buttonState4.right
        anchors.leftMargin: _margins

        id    : _comboboxTable4
        width : _Width
        height: _Height
        model : controller.tableList4
        onCurrentTextChanged: {
            controller.setFieldList4(_comboboxTable4.currentText)
        }
    }
    ComboBox{
        anchors.top       : _comboboxTable4.top
        anchors.left      : _comboboxTable4.right
        anchors.leftMargin: _margins

        id    : _comboboxField4
        width : _Width
        height: _Height
        model : controller.fieldList4
        onCurrentTextChanged: {
            controller.setField4(_comboboxField4.currentText)
        }
    }
    TextField{
        anchors.top       : _comboboxField4.top
        anchors.left      : _comboboxField4.right
        anchors.leftMargin: _margins

        id   : _textfieldScale4
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.scale4
        onAccepted: {
            controller.setScale4(_textfieldScale4.text)
        }
    }
    TextField{
        anchors.top       : _textfieldScale4.top
        anchors.left      : _textfieldScale4.right
        anchors.leftMargin: _margins

        id   : _textfieldX4
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetX4
        onAccepted: {
            controller.setOffsetX4(_textfieldX4.text)
        }
    }
    TextField{
        anchors.top       : _textfieldX4.top
        anchors.left      : _textfieldX4.right
        anchors.leftMargin: _margins

        id   : _textfieldY4
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetY4
        onAccepted: {
            controller.setOffsetY4(_textfieldY4.text)
        }
    }
    ComboBox{
        anchors.top       : _textfieldY4.top
        anchors.left      : _textfieldY4.right
        anchors.leftMargin: _margins

        id    : _comboboxLine4
        width : _Width
        height: _Height
        model : controller.lineList
        onCurrentTextChanged: {
            controller.setLineStyle4(parseInt(_comboboxLine4.currentIndex))
        }
    }
    ComboBox{
        anchors.top       : _comboboxLine4.top
        anchors.left      : _comboboxLine4.right
        anchors.leftMargin: _margins

        id    : _comboboxColor4
        width : _Width
        height: _Height
        model: controller.colorList4
        onCurrentTextChanged: {
            controller.setLineColor4(_comboboxColor4.currentText)
        }
    }

    // Row 5
    Button{
        anchors.top       : _buttonState4.bottom
        anchors.topMargin : _margins
        anchors.left      : _buttonState4.left

        id: _buttonState5
        width : _Width
        height: _Height
        text  : {
            if(controller.visible5){
                qsTr("5")
            }else{
                qsTr("hide")
            }
        }
        onClicked:{
            controller.setVisible5(!controller.visible5)
        }
    }
    ComboBox{
        anchors.top       : _buttonState5.top
        anchors.left      : _buttonState5.right
        anchors.leftMargin: _margins

        id    : _comboboxTable5
        width : _Width
        height: _Height
        model : controller.tableList5
        onCurrentTextChanged: {
            controller.setFieldList5(_comboboxTable5.currentText)
        }
    }
    ComboBox{
        anchors.top       : _comboboxTable5.top
        anchors.left      : _comboboxTable5.right
        anchors.leftMargin: _margins

        id    : _comboboxField5
        width : _Width
        height: _Height
        model : controller.fieldList5
        onCurrentTextChanged: {
            controller.setField5(_comboboxField5.currentText)
        }
    }
    TextField{
        anchors.top       : _comboboxField5.top
        anchors.left      : _comboboxField5.right
        anchors.leftMargin: _margins

        id   : _textfieldScale5
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.scale5
        onAccepted: {
            controller.setScale5(_textfieldScale5.text)
        }
    }
    TextField{
        anchors.top       : _textfieldScale5.top
        anchors.left      : _textfieldScale5.right
        anchors.leftMargin: _margins

        id   : _textfieldX5
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetX5
        onAccepted: {
            controller.setOffsetX5(_textfieldX5.text)
        }
    }
    TextField{
        anchors.top       : _textfieldX5.top
        anchors.left      : _textfieldX5.right
        anchors.leftMargin: _margins

        id   : _textfieldY5
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetY5
        onAccepted: {
            controller.setOffsetY5(_textfieldY5.text)
        }
    }
    ComboBox{
        anchors.top       : _textfieldY5.top
        anchors.left      : _textfieldY5.right
        anchors.leftMargin: _margins

        id    : _comboboxLine5
        width : _Width
        height: _Height
        model : controller.lineList
        onCurrentTextChanged: {
            controller.setLineStyle5(parseInt(_comboboxLine5.currentIndex))
        }
    }
    ComboBox{
        anchors.top       : _comboboxLine5.top
        anchors.left      : _comboboxLine5.right
        anchors.leftMargin: _margins

        id    : _comboboxColor5
        width : _Width
        height: _Height
        model: controller.colorList5
        onCurrentTextChanged: {
            controller.setLineColor5(_comboboxColor5.currentText)
        }
    }

    // Row 6
    Button{
        anchors.top       : _buttonState5.bottom
        anchors.topMargin : _margins
        anchors.left      : _buttonState5.left

        id: _buttonState6
        width : _Width
        height: _Height
        text  : {
            if(controller.visible6){
                qsTr("6")
            }else{
                qsTr("hide")
            }
        }
        onClicked:{
            controller.setVisible6(!controller.visible6)
        }
    }
    ComboBox{
        anchors.top       : _buttonState6.top
        anchors.left      : _buttonState6.right
        anchors.leftMargin: _margins

        id    : _comboboxTable6
        width : _Width
        height: _Height
        model : controller.tableList6
        onCurrentTextChanged: {
            controller.setFieldList6(_comboboxTable6.currentText)
        }
    }
    ComboBox{
        anchors.top       : _comboboxTable6.top
        anchors.left      : _comboboxTable6.right
        anchors.leftMargin: _margins

        id    : _comboboxField6
        width : _Width
        height: _Height
        model : controller.fieldList6
        onCurrentTextChanged: {
            controller.setField6(_comboboxField6.currentText)
        }
    }
    TextField{
        anchors.top       : _comboboxField6.top
        anchors.left      : _comboboxField6.right
        anchors.leftMargin: _margins

        id   : _textfieldScale6
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.scale6
        onAccepted: {
            controller.setScale6(_textfieldScale6.text)
        }
    }
    TextField{
        anchors.top       : _textfieldScale6.top
        anchors.left      : _textfieldScale6.right
        anchors.leftMargin: _margins

        id   : _textfieldX6
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetX6
        onAccepted: {
            controller.setOffsetX6(_textfieldX6.text)
        }
    }
    TextField{
        anchors.top       : _textfieldX6.top
        anchors.left      : _textfieldX6.right
        anchors.leftMargin: _margins

        id   : _textfieldY6
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetY6
        onAccepted: {
            controller.setOffsetY6(_textfieldY6.text)
        }
    }
    ComboBox{
        anchors.top       : _textfieldY6.top
        anchors.left      : _textfieldY6.right
        anchors.leftMargin: _margins

        id    : _comboboxLine6
        width : _Width
        height: _Height
        model : controller.lineList
        onCurrentTextChanged: {
            controller.setLineStyle6(parseInt(_comboboxLine6.currentIndex))
        }
    }
    ComboBox{
        anchors.top       : _comboboxLine6.top
        anchors.left      : _comboboxLine6.right
        anchors.leftMargin: _margins

        id    : _comboboxColor6
        width : _Width
        height: _Height
        model: controller.colorList6
        onCurrentTextChanged: {
            controller.setLineColor6(_comboboxColor6.currentText)
        }
    }

    // Row 7
    Button{
        anchors.top       : _buttonState6.bottom
        anchors.topMargin : _margins
        anchors.left      : _buttonState6.left

        id: _buttonState7
        width : _Width
        height: _Height
        text  : {
            if(controller.visible7){
                qsTr("7")
            }else{
                qsTr("hide")
            }
        }
        onClicked:{
            controller.setVisible7(!controller.visible7)
        }
    }
    ComboBox{
        anchors.top       : _buttonState7.top
        anchors.left      : _buttonState7.right
        anchors.leftMargin: _margins

        id    : _comboboxTable7
        width : _Width
        height: _Height
        model : controller.tableList7
        onCurrentTextChanged: {
            controller.setFieldList7(_comboboxTable7.currentText)
        }
    }
    ComboBox{
        anchors.top       : _comboboxTable7.top
        anchors.left      : _comboboxTable7.right
        anchors.leftMargin: _margins

        id    : _comboboxField7
        width : _Width
        height: _Height
        model : controller.fieldList7
        onCurrentTextChanged: {
            controller.setField7(_comboboxField7.currentText)
        }
    }
    TextField{
        anchors.top       : _comboboxField7.top
        anchors.left      : _comboboxField7.right
        anchors.leftMargin: _margins

        id   : _textfieldScale7
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.scale7
        onAccepted: {
            controller.setScale7(_textfieldScale7.text)
        }
    }
    TextField{
        anchors.top       : _textfieldScale7.top
        anchors.left      : _textfieldScale7.right
        anchors.leftMargin: _margins

        id   : _textfieldX7
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetX7
        onAccepted: {
            controller.setOffsetX7(_textfieldX7.text)
        }
    }
    TextField{
        anchors.top       : _textfieldX7.top
        anchors.left      : _textfieldX7.right
        anchors.leftMargin: _margins

        id   : _textfieldY7
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetY7
        onAccepted: {
            controller.setOffsetY7(_textfieldY7.text)
        }
    }
    ComboBox{
        anchors.top       : _textfieldY7.top
        anchors.left      : _textfieldY7.right
        anchors.leftMargin: _margins

        id    : _comboboxLine7
        width : _Width
        height: _Height
        model : controller.lineList
        onCurrentTextChanged: {
            controller.setLineStyle7(parseInt(_comboboxLine7.currentIndex))
        }
    }
    ComboBox{
        anchors.top       : _comboboxLine7.top
        anchors.left      : _comboboxLine7.right
        anchors.leftMargin: _margins

        id    : _comboboxColor7
        width : _Width
        height: _Height
        model: controller.colorList7
        onCurrentTextChanged: {
            controller.setLineColor7(_comboboxColor7.currentText)
        }
    }

    // Row 8
    Button{
        anchors.top       : _buttonState7.bottom
        anchors.topMargin : _margins
        anchors.left      : _buttonState7.left

        id: _buttonState8
        width : _Width
        height: _Height
        text  : {
            if(controller.visible8){
                qsTr("8")
            }else{
                qsTr("hide")
            }
        }
        onClicked:{
            controller.setVisible8(!controller.visible8)
        }
    }
    ComboBox{
        anchors.top       : _buttonState8.top
        anchors.left      : _buttonState8.right
        anchors.leftMargin: _margins

        id    : _comboboxTable8
        width : _Width
        height: _Height
        model : controller.tableList8
        onCurrentTextChanged: {
            controller.setFieldList8(_comboboxTable8.currentText)
        }
    }
    ComboBox{
        anchors.top       : _comboboxTable8.top
        anchors.left      : _comboboxTable8.right
        anchors.leftMargin: _margins

        id    : _comboboxField8
        width : _Width
        height: _Height
        model : controller.fieldList8
        onCurrentTextChanged: {
            controller.setField8(_comboboxField8.currentText)
        }
    }
    TextField{
        anchors.top       : _comboboxField8.top
        anchors.left      : _comboboxField8.right
        anchors.leftMargin: _margins

        id   : _textfieldScale8
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.scale8
        onAccepted: {
            controller.setScale8(_textfieldScale8.text)
        }
    }
    TextField{
        anchors.top       : _textfieldScale8.top
        anchors.left      : _textfieldScale8.right
        anchors.leftMargin: _margins

        id   : _textfieldX8
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetX8
        onAccepted: {
            controller.setOffsetX8(_textfieldX8.text)
        }
    }
    TextField{
        anchors.top       : _textfieldX8.top
        anchors.left      : _textfieldX8.right
        anchors.leftMargin: _margins

        id   : _textfieldY8
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetY8
        onAccepted: {
            controller.setOffsetY8(_textfieldY8.text)
        }
    }
    ComboBox{
        anchors.top       : _textfieldY8.top
        anchors.left      : _textfieldY8.right
        anchors.leftMargin: _margins

        id    : _comboboxLine8
        width : _Width
        height: _Height
        model : controller.lineList
        onCurrentTextChanged: {
            controller.setLineStyle8(parseInt(_comboboxLine8.currentIndex))
        }
    }
    ComboBox{
        anchors.top       : _comboboxLine8.top
        anchors.left      : _comboboxLine8.right
        anchors.leftMargin: _margins

        id    : _comboboxColor8
        width : _Width
        height: _Height
        model: controller.colorList8
        onCurrentTextChanged: {
            controller.setLineColor8(_comboboxColor8.currentText)
        }
    }

    // Row 9
    Button{
        anchors.top       : _buttonState8.bottom
        anchors.topMargin : _margins
        anchors.left      : _buttonState8.left

        id: _buttonState9
        width : _Width
        height: _Height
        text  : {
            if(controller.visible9){
                qsTr("9")
            }else{
                qsTr("hide")
            }
        }
        onClicked:{
            controller.setVisible9(!controller.visible9)
        }
    }
    ComboBox{
        anchors.top       : _buttonState9.top
        anchors.left      : _buttonState9.right
        anchors.leftMargin: _margins

        id    : _comboboxTable9
        width : _Width
        height: _Height
        model : controller.tableList9
        onCurrentTextChanged: {
            controller.setFieldList9(_comboboxTable9.currentText)
        }
    }
    ComboBox{
        anchors.top       : _comboboxTable9.top
        anchors.left      : _comboboxTable9.right
        anchors.leftMargin: _margins

        id    : _comboboxField9
        width : _Width
        height: _Height
        model : controller.fieldList9
        onCurrentTextChanged: {
            controller.setField9(_comboboxField9.currentText)
        }
    }
    TextField{
        anchors.top       : _comboboxField9.top
        anchors.left      : _comboboxField9.right
        anchors.leftMargin: _margins

        id   : _textfieldScale9
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.scale9
        onAccepted: {
            controller.setScale9(_textfieldScale9.text)
        }
    }
    TextField{
        anchors.top       : _textfieldScale9.top
        anchors.left      : _textfieldScale9.right
        anchors.leftMargin: _margins

        id   : _textfieldX9
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetX9
        onAccepted: {
            controller.setOffsetX9(_textfieldX9.text)
        }
    }
    TextField{
        anchors.top       : _textfieldX9.top
        anchors.left      : _textfieldX9.right
        anchors.leftMargin: _margins

        id   : _textfieldY9
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetY9
        onAccepted: {
            controller.setOffsetY9(_textfieldY9.text)
        }
    }
    ComboBox{
        anchors.top       : _textfieldY9.top
        anchors.left      : _textfieldY9.right
        anchors.leftMargin: _margins

        id    : _comboboxLine9
        width : _Width
        height: _Height
        model : controller.lineList
        onCurrentTextChanged: {
            controller.setLineStyle9(parseInt(_comboboxLine9.currentIndex))
        }
    }
    ComboBox{
        anchors.top       : _comboboxLine9.top
        anchors.left      : _comboboxLine9.right
        anchors.leftMargin: _margins

        id    : _comboboxColor9
        width : _Width
        height: _Height
        model: controller.colorList9
        onCurrentTextChanged: {
            controller.setLineColor9(_comboboxColor9.currentText)
        }
    }

    // Row 10
    Button{
        anchors.top       : _buttonState9.bottom
        anchors.topMargin : _margins
        anchors.left      : _buttonState9.left

        id: _buttonState10
        width : _Width
        height: _Height
        text  : {
            if(controller.visible10){
                qsTr("10")
            }else{
                qsTr("hide")
            }
        }
        onClicked:{
            controller.setVisible10(!controller.visible10)
        }
    }
    ComboBox{
        anchors.top       : _buttonState10.top
        anchors.left      : _buttonState10.right
        anchors.leftMargin: _margins

        id    : _comboboxTable10
        width : _Width
        height: _Height
        model : controller.tableList10
        onCurrentTextChanged: {
            controller.setFieldList10(_comboboxTable10.currentText)
        }
    }
    ComboBox{
        anchors.top       : _comboboxTable10.top
        anchors.left      : _comboboxTable10.right
        anchors.leftMargin: _margins

        id    : _comboboxField10
        width : _Width
        height: _Height
        model : controller.fieldList10
        onCurrentTextChanged: {
            controller.setField10(_comboboxField10.currentText)
        }
    }
    TextField{
        anchors.top       : _comboboxField10.top
        anchors.left      : _comboboxField10.right
        anchors.leftMargin: _margins

        id   : _textfieldScale10
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.scale10
        onAccepted: {
            controller.setScale10(_textfieldScale10.text)
        }
    }
    TextField{
        anchors.top       : _textfieldScale10.top
        anchors.left      : _textfieldScale10.right
        anchors.leftMargin: _margins

        id   : _textfieldX10
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetX10
        onAccepted: {
            controller.setOffsetX10(_textfieldX10.text)
        }
    }
    TextField{
        anchors.top       : _textfieldX10.top
        anchors.left      : _textfieldX10.right
        anchors.leftMargin: _margins

        id   : _textfieldY10
        width: _Width
        height: _Height

        placeholderText: qsTr("")
        text: controller.offsetY10
        onAccepted: {
            controller.setOffsetY10(_textfieldY10.text)
        }
    }
    ComboBox{
        anchors.top       : _textfieldY10.top
        anchors.left      : _textfieldY10.right
        anchors.leftMargin: _margins

        id    : _comboboxLine10
        width : _Width
        height: _Height
        model : controller.lineList
        onCurrentTextChanged: {
            controller.setLineStyle10(parseInt(_comboboxLine10.currentIndex))
        }
    }
    ComboBox{
        anchors.top       : _comboboxLine10.top
        anchors.left      : _comboboxLine10.right
        anchors.leftMargin: _margins

        id    : _comboboxColor10
        width : _Width
        height: _Height
        model: controller.colorList10
        onCurrentTextChanged: {
            controller.setLineColor10(_comboboxColor10.currentText)
        }
    }
}
