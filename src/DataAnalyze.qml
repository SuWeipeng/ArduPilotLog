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
                Layout.fillWidth: true
                model: controller.tableList1
                onActivated: controller.setFieldList1(model[index])
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList1
                onActivated: controller.setField1(model[index])
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
                onCurrentIndexChanged: controller.setLineStyle1(currentIndex)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.colorList1
                onActivated: controller.setLineColor1(model[index])
            }

            // --- 第 2 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible2 ? qsTr("2") : qsTr("hide")
                onClicked: controller.setVisible2(!controller.visible2)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.tableList2
                onActivated: controller.setFieldList2(model[index])
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList2
                onActivated: controller.setField2(model[index])
            }
            TextField { Layout.fillWidth: true; text: controller.scale2; onAccepted: controller.setScale2(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX2; onAccepted: controller.setOffsetX2(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY2; onAccepted: controller.setOffsetY2(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: controller.setLineStyle2(currentIndex)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.colorList2
                onActivated: controller.setLineColor2(model[index])
            }

            // --- 第 3 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible3 ? qsTr("3") : qsTr("hide")
                onClicked: controller.setVisible3(!controller.visible3)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.tableList3
                onActivated: controller.setFieldList3(model[index])
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList3
                onActivated: controller.setField3(model[index])
            }
            TextField { Layout.fillWidth: true; text: controller.scale3; onAccepted: controller.setScale3(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX3; onAccepted: controller.setOffsetX3(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY3; onAccepted: controller.setOffsetY3(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: controller.setLineStyle3(currentIndex)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.colorList3
                onActivated: controller.setLineColor3(model[index])
            }

            // --- 第 4 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible4 ? qsTr("4") : qsTr("hide")
                onClicked: controller.setVisible4(!controller.visible4)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.tableList4
                onActivated: controller.setFieldList4(model[index])
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList4
                onActivated: controller.setField4(model[index])
            }
            TextField { Layout.fillWidth: true; text: controller.scale4; onAccepted: controller.setScale4(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX4; onAccepted: controller.setOffsetX4(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY4; onAccepted: controller.setOffsetY4(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: controller.setLineStyle4(currentIndex)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.colorList4
                onActivated: controller.setLineColor4(model[index])
            }

            // --- 第 5 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible5 ? qsTr("5") : qsTr("hide")
                onClicked: controller.setVisible5(!controller.visible5)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.tableList5
                onActivated: controller.setFieldList5(model[index])
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList5
                onActivated: controller.setField5(model[index])
            }
            TextField { Layout.fillWidth: true; text: controller.scale5; onAccepted: controller.setScale5(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX5; onAccepted: controller.setOffsetX5(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY5; onAccepted: controller.setOffsetY5(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: controller.setLineStyle5(currentIndex)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.colorList5
                onActivated: controller.setLineColor5(model[index])
            }

            // --- 第 6 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible6 ? qsTr("6") : qsTr("hide")
                onClicked: controller.setVisible6(!controller.visible6)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.tableList6
                onActivated: controller.setFieldList6(model[index])
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList6
                onActivated: controller.setField6(model[index])
            }
            TextField { Layout.fillWidth: true; text: controller.scale6; onAccepted: controller.setScale6(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX6; onAccepted: controller.setOffsetX6(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY6; onAccepted: controller.setOffsetY6(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: controller.setLineStyle6(currentIndex)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.colorList6
                onActivated: controller.setLineColor6(model[index])
            }

            // --- 第 7 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible7 ? qsTr("7") : qsTr("hide")
                onClicked: controller.setVisible7(!controller.visible7)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.tableList7
                onActivated: controller.setFieldList7(model[index])
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList7
                onActivated: controller.setField7(model[index])
            }
            TextField { Layout.fillWidth: true; text: controller.scale7; onAccepted: controller.setScale7(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX7; onAccepted: controller.setOffsetX7(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY7; onAccepted: controller.setOffsetY7(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: controller.setLineStyle7(currentIndex)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.colorList7
                onActivated: controller.setLineColor7(model[index])
            }

            // --- 第 8 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible8 ? qsTr("8") : qsTr("hide")
                onClicked: controller.setVisible8(!controller.visible8)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.tableList8
                onActivated: controller.setFieldList8(model[index])
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList8
                onActivated: controller.setField8(model[index])
            }
            TextField { Layout.fillWidth: true; text: controller.scale8; onAccepted: controller.setScale8(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX8; onAccepted: controller.setOffsetX8(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY8; onAccepted: controller.setOffsetY8(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: controller.setLineStyle8(currentIndex)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.colorList8
                onActivated: controller.setLineColor8(model[index])
            }

            // --- 第 9 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible9 ? qsTr("9") : qsTr("hide")
                onClicked: controller.setVisible9(!controller.visible9)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.tableList9
                onActivated: controller.setFieldList9(model[index])
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList9
                onActivated: controller.setField9(model[index])
            }
            TextField { Layout.fillWidth: true; text: controller.scale9; onAccepted: controller.setScale9(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX9; onAccepted: controller.setOffsetX9(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY9; onAccepted: controller.setOffsetY9(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: controller.setLineStyle9(currentIndex)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.colorList9
                onActivated: controller.setLineColor9(model[index])
            }

            // --- 第 10 行: 数据 ---
            Button {
                Layout.fillWidth: true
                text: controller.visible10 ? qsTr("10") : qsTr("hide")
                onClicked: controller.setVisible10(!controller.visible10)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.tableList10
                onActivated: controller.setFieldList10(model[index])
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.fieldList10
                onActivated: controller.setField10(model[index])
            }
            TextField { Layout.fillWidth: true; text: controller.scale10; onAccepted: controller.setScale10(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetX10; onAccepted: controller.setOffsetX10(text) }
            TextField { Layout.fillWidth: true; text: controller.offsetY10; onAccepted: controller.setOffsetY10(text) }
            ComboBox {
                Layout.fillWidth: true
                model: controller.lineList
                onCurrentIndexChanged: controller.setLineStyle10(currentIndex)
            }
            ComboBox {
                Layout.fillWidth: true
                model: controller.colorList10
                onActivated: controller.setLineColor10(model[index])
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
