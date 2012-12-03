!include("../DeviceControl.pri") {
    error("../DeviceControl.pri not found")
}

TARGET = utWater

TEMPLATE = app

INCLUDEPATH +=	../../../../Components/ \
                ../../../../../Common/Components/ \
                ../SlaveModules/ \

DEFINES +=  DEVICECONTROL_FUNCTIONMODULEPRIVATE_H \
            DEVICECONTROL_BASEMODULEPRIVATE_H \
            DEVICECONTROL_STEPPERMOTORPRIVATE_H \
            DEVICECONTROL_DIGITALINPUTPRIVATE_H \
            DEVICECONTROL_DIGITALOUTPUTPRIVATE_H \
            DEVICECONTROL_TEMPERATURECONTROLPRIVATE_H \
            DEVICECONTROL_RFID11785PRIVATE_H \
            DEVICEPROCESSING_H

DEFINES +=  Q_UNIT_TEST

#QMAKE_CXXFLAGS += Simulator.h

HEADERS +=  ../../Include/Devices/DeviceState.h \
            ../../Include/Devices/DeviceWater.h \
            ../../Include/Devices/DeviceBase.h \
            ../SlaveModules/*.h \

SOURCES +=  TestWater.cpp \
            ../../Source/Devices/DeviceState.cpp \
            ../../Source/Devices/DeviceWater.cpp \
            ../../Source/Devices/DeviceBase.cpp \
            ../SlaveModules/*.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
