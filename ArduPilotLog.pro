#-------------------------------------------------
#
# Project created by QtCreator 2017-12-23T11:01:59
#
#-------------------------------------------------

QT       += core gui sql qml quick quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

include(APLCommon.pri)

TARGET = ArduPilotLog
TEMPLATE = app

DebugBuild {
    DESTDIR  = $${OUT_PWD}/debug
} else {
    DESTDIR  = $${OUT_PWD}/release
}

LinuxBuild {
    CONFIG  += qesp_linux_udev
}

WindowsBuild {
    RC_ICONS = resources/icons/ardupilotlog.ico
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    APLLoggingCategory.cpp \
    APLDockWidget.cpp \
    src/Dialog.cpp \
    src/APLRead.cpp \
    src/APLDB.cpp \
    qcustomplot.cpp \
    Plot2D.cpp \
    src/DataAnalyzeController.cpp \
    src/APLQmlWidgetHolder.cpp \

HEADERS += \
        mainwindow.h \
    APLLoggingCategory.h \
    APLDockWidget.h \
    src/Dialog.h \
    src/APLRead.h \
    src/LogStructure.h \
    src/APLDB.h \
    qcustomplot.h \
    src/DataAnalyzeController.h \
    src/DataAnalyze.h \
    src/APLQmlWidgetHolder.h \

FORMS += \
        mainwindow.ui \
    src/APLQmlWidgetHolder.ui

include(APLSetup.pri)

DISTFILES += \
    APLCommon.pri

RESOURCES += \
    $$PWD/aplresources.qrc \
    $$PWD/ardupilotlog.qrc
