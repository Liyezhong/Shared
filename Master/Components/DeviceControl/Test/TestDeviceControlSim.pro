!include("DeviceControl.pri"):error("DeviceControl.pri not found")

QT       += testlib \
            xml

QT       -= gui

TARGET = TestDeviceControlSim
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../..
INCLUDEPATH += ../Include
INCLUDEPATH += ../../../../Common/Components/ \
               ../hwconfig/ \
               ../../../../../Shared/ExternalPackages/xsde/libxsde


SOURCES += TestDeviceControlSim.cpp \

DEFINES += SRCDIR=\\\"$$PWD/\\\"

UseLibs(Global DataManager DeviceControl)
unix:!macx: LIBS += -L../../../../../Shared/ExternalPackages/xsde/libxsde/xsde/ -lxsde

INCLUDEPATH += $../../../../../Shared/ExternalPackages/xsde/libxsde
DEPENDPATH += $../../../../../Shared/ExternalPackages/xsde/libxsde

unix:!macx: PRE_TARGETDEPS += ../../../../../Shared/ExternalPackages/xsde/libxsde/xsde/libxsde.a
