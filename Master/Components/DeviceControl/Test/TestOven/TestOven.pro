!include("../DeviceControl.pri") {
    error("../DeviceControl.pri not found")
}

TARGET = utOven

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
            ../../Include/Devices/DeviceOven.h \
            ../../Include/Devices/DeviceBase.h \
            ../../Include/Devices/FmStepperInit.h \
            ../../Include/Devices/FmStepperMove.h \
            ../../Include/Devices/MoveOpenClosed.h \
            ../../Include/Devices/TemperatureState.h \
            ../../Include/Devices/SetDrawerLed.h \
            ../SlaveModules/*.h \

SOURCES +=  TestOven.cpp \
            ../../Source/Devices/DeviceState.cpp \
            ../../Source/Devices/DeviceOven.cpp \
            ../../Source/Devices/DeviceBase.cpp \
            ../../Source/Devices/FmStepperInit.cpp \
            ../../Source/Devices/FmStepperMove.cpp \
            ../../Source/Devices/MoveOpenClosed.cpp \
            ../../Source/Devices/TemperatureState.cpp \
            ../../Source/Devices/SetDrawerLed.cpp \
            ../SlaveModules/*.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
