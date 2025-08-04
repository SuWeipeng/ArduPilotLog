import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

import ArduPilotLog.Controllers 1.0

Rectangle {
    id: dataAnalyze

    width: 650
    height: 500

    DataAnalyzeController {
        id: controller
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        GridLayout {
            columns: 8
            columnSpacing: 5
            rowSpacing: 5

            Layout.fillWidth: true
            Layout.fillHeight: true

            // --- 第 0 行: 表头 ---
            Label { text: qsTr("State"); font.bold: true; Layout.alignment: Qt.AlignCenter }
            Label { text: qsTr("Table"); font.bold: true; Layout.alignment: Qt.AlignCenter }
            Label { text: qsTr("Field"); font.bold: true; Layout.alignment: Qt.AlignCenter }
            Label { text: qsTr("Scale"); font.bold: true; Layout.alignment: Qt.AlignCenter }
            Label { text: qsTr("X+"); font.bold: true; Layout.alignment: Qt.AlignCenter }
            Label { text: qsTr("Y+"); font.bold: true; Layout.alignment: Qt.AlignCenter }
            Label { text: qsTr("Line"); font.bold: true; Layout.alignment: Qt.AlignCenter }
            Label { text: qsTr("Color"); font.bold: true; Layout.alignment: Qt.AlignCenter }

            // --- 第 1 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible1 ? qsTr("1") : qsTr("hide")
                onClicked: controller.setVisible1(!controller.visible1)
            }
            ComboBox {
                id    : _comboboxTable1
                Layout.fillWidth: true
                model: controller.tableList1
                onCurrentTextChanged: controller.setFieldList1(currentText)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList1
                onActivated: function(index) { controller.setField1(model[index]) }
            }
            TextField {
                Layout.fillWidth: true
                text: controller.scale1
                placeholderText: "1.0"
                onAccepted: controller.setScale1(text)
            }
            TextField {
                Layout.fillWidth: true
                text: controller.offsetX1
                placeholderText: "0.0"
                onAccepted: controller.setOffsetX1(text)
            }
            TextField {
                Layout.fillWidth: true
                text: controller.offsetY1
                placeholderText: "0.0"
                onAccepted: controller.setOffsetY1(text)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: function() { controller.setLineStyle1(currentIndex) }
            }
            ComboBox {
                id: _comboboxColor1
                Layout.fillWidth: true
                model: controller.colorList1
                onCurrentTextChanged: controller.setLineColor1(currentText)
            }

            // --- 第 2 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible2 ? qsTr("2") : qsTr("hide")
                onClicked: controller.setVisible2(!controller.visible2)
            }
            ComboBox {
                id: _comboboxTable2
                Layout.fillWidth: true
                model: controller.tableList2
                onCurrentTextChanged: controller.setFieldList2(currentText)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList2
                onActivated: function(index) { controller.setField2(model[index]) }
            }
            TextField { Layout.fillWidth: true; text: controller.scale2; onAccepted: controller.setScale2(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX2; onAccepted: controller.setOffsetX2(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY2; onAccepted: controller.setOffsetY2(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: function() { controller.setLineStyle2(currentIndex) }
            }
            ComboBox {
                id: _comboboxColor2
                Layout.fillWidth: true
                model: controller.colorList2
                onCurrentTextChanged: controller.setLineColor2(currentText)
            }

            // --- 第 3 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible3 ? qsTr("3") : qsTr("hide")
                onClicked: controller.setVisible3(!controller.visible3)
            }
            ComboBox {
                id: _comboboxTable3
                Layout.fillWidth: true
                model: controller.tableList3
                onCurrentTextChanged: controller.setFieldList3(currentText)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList3
                onActivated: function(index) { controller.setField3(model[index]) }
            }
            TextField { Layout.fillWidth: true; text: controller.scale3; onAccepted: controller.setScale3(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX3; onAccepted: controller.setOffsetX3(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY3; onAccepted: controller.setOffsetY3(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: function() { controller.setLineStyle3(currentIndex) }
            }
            ComboBox {
                id: _comboboxColor3
                Layout.fillWidth: true
                model: controller.colorList3
                onCurrentTextChanged: controller.setLineColor3(currentText)
            }

            // --- 第 4 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible4 ? qsTr("4") : qsTr("hide")
                onClicked: controller.setVisible4(!controller.visible4)
            }
            ComboBox {
                id: _comboboxTable4
                Layout.fillWidth: true
                model: controller.tableList4
                onCurrentTextChanged: controller.setFieldList4(currentText)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList4
                onActivated: function(index) { controller.setField4(model[index]) }
            }
            TextField { Layout.fillWidth: true; text: controller.scale4; onAccepted: controller.setScale4(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX4; onAccepted: controller.setOffsetX4(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY4; onAccepted: controller.setOffsetY4(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: function() { controller.setLineStyle4(currentIndex) }
            }
            ComboBox {
                id: _comboboxColor4
                Layout.fillWidth: true
                model: controller.colorList4
                onCurrentTextChanged: controller.setLineColor4(currentText)
            }

            // --- 第 5 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible5 ? qsTr("5") : qsTr("hide")
                onClicked: controller.setVisible5(!controller.visible5)
            }
            ComboBox {
                id: _comboboxTable5
                Layout.fillWidth: true
                model: controller.tableList5
                onCurrentTextChanged: controller.setFieldList5(currentText)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList5
                onActivated: function(index) { controller.setField5(model[index]) }
            }
            TextField { Layout.fillWidth: true; text: controller.scale5; onAccepted: controller.setScale5(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX5; onAccepted: controller.setOffsetX5(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY5; onAccepted: controller.setOffsetY5(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: function() { controller.setLineStyle5(currentIndex) }
            }
            ComboBox {
                id: _comboboxColor5
                Layout.fillWidth: true
                model: controller.colorList5
                onCurrentTextChanged: controller.setLineColor5(currentText)
            }

            // --- 第 6 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible6 ? qsTr("6") : qsTr("hide")
                onClicked: controller.setVisible6(!controller.visible6)
            }
            ComboBox {
                id: _comboboxTable6
                Layout.fillWidth: true
                model: controller.tableList6
                onCurrentTextChanged: controller.setFieldList6(currentText)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList6
                onActivated: function(index) { controller.setField6(model[index]) }
            }
            TextField { Layout.fillWidth: true; text: controller.scale6; onAccepted: controller.setScale6(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX6; onAccepted: controller.setOffsetX6(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY6; onAccepted: controller.setOffsetY6(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: function() { controller.setLineStyle6(currentIndex) }
            }
            ComboBox {
                id: _comboboxColor6
                Layout.fillWidth: true
                model: controller.colorList6
                onCurrentTextChanged: controller.setLineColor6(currentText)
            }

            // --- 第 7 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible7 ? qsTr("7") : qsTr("hide")
                onClicked: controller.setVisible7(!controller.visible7)
            }
            ComboBox {
                id: _comboboxTable7
                Layout.fillWidth: true
                model: controller.tableList7
                onCurrentTextChanged: controller.setFieldList7(currentText)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList7
                onActivated: function(index) { controller.setField7(model[index]) }
            }
            TextField { Layout.fillWidth: true; text: controller.scale7; onAccepted: controller.setScale7(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX7; onAccepted: controller.setOffsetX7(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY7; onAccepted: controller.setOffsetY7(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: function() { controller.setLineStyle7(currentIndex) }
            }
            ComboBox {
                id: _comboboxColor7
                Layout.fillWidth: true
                model: controller.colorList7
                onCurrentTextChanged: controller.setLineColor7(currentText)
            }

            // --- 第 8 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible8 ? qsTr("8") : qsTr("hide")
                onClicked: controller.setVisible8(!controller.visible8)
            }
            ComboBox {
                id: _comboboxTable8
                Layout.fillWidth: true
                model: controller.tableList8
                onCurrentTextChanged: controller.setFieldList8(currentText)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList8
                onActivated: function(index) { controller.setField8(model[index]) }
            }
            TextField { Layout.fillWidth: true; text: controller.scale8; onAccepted: controller.setScale8(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX8; onAccepted: controller.setOffsetX8(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY8; onAccepted: controller.setOffsetY8(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: function() { controller.setLineStyle8(currentIndex) }
            }
            ComboBox {
                id: _comboboxColor8
                Layout.fillWidth: true
                model: controller.colorList8
                onCurrentTextChanged: controller.setLineColor8(currentText)
            }

            // --- 第 9 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible9 ? qsTr("9") : qsTr("hide")
                onClicked: controller.setVisible9(!controller.visible9)
            }
            ComboBox {
                id: _comboboxTable9
                Layout.fillWidth: true
                model: controller.tableList9
                onCurrentTextChanged: controller.setFieldList9(currentText)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList9
                onActivated: function(index) { controller.setField9(model[index]) }
            }
            TextField { Layout.fillWidth: true; text: controller.scale9; onAccepted: controller.setScale9(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX9; onAccepted: controller.setOffsetX9(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY9; onAccepted: controller.setOffsetY9(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: function() { controller.setLineStyle9(currentIndex) }
            }
            ComboBox {
                id: _comboboxColor9
                Layout.fillWidth: true
                model: controller.colorList9
                onCurrentTextChanged: controller.setLineColor9(currentText)
            }

            // --- 第 10 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible10 ? qsTr("10") : qsTr("hide")
                onClicked: controller.setVisible10(!controller.visible10)
            }
            ComboBox {
                id: _comboboxTable10
                Layout.fillWidth: true
                model: controller.tableList10
                onCurrentTextChanged: controller.setFieldList10(currentText)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList10
                onActivated: function(index) { controller.setField10(model[index]) }
            }
            TextField { Layout.fillWidth: true; text: controller.scale10; onAccepted: controller.setScale10(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX10; onAccepted: controller.setOffsetX10(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY10; onAccepted: controller.setOffsetY10(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: function() { controller.setLineStyle10(currentIndex) }
            }
            ComboBox {
                id: _comboboxColor10
                Layout.fillWidth: true
                model: controller.colorList10
                onCurrentTextChanged: controller.setLineColor10(currentText)
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight

            Button{
                id: _buttonInit
                text: qsTr("Init")
                onClicked:{
                    controller.init()
                    controller.setVisible1(false)
                    controller.setVisible2(false)
                    controller.setVisible3(false)
                    controller.setVisible4(false)
                    controller.setVisible5(false)
                    controller.setVisible6(false)
                    controller.setVisible7(false)
                    controller.setVisible8(false)
                    controller.setVisible9(false)
                    controller.setVisible10(false)
                }
            }
            Button{
                id: _buttonHideAll
                text: qsTr("Hide All")
                onClicked:{
                    controller.setVisible1(false)
                    controller.setVisible2(false)
                    controller.setVisible3(false)
                    controller.setVisible4(false)
                    controller.setVisible5(false)
                    controller.setVisible6(false)
                    controller.setVisible7(false)
                    controller.setVisible8(false)
                    controller.setVisible9(false)
                    controller.setVisible10(false)
                }
            }
        }
    }
}
