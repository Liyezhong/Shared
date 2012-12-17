!include("../DeviceControl.pri") {
    error("../DeviceControl.pri not found")
}

TARGET = utXyz

TEMPLATE = app

INCLUDEPATH +=	../../../../Components/ \
                ../../../../../Common/Components/ \
                ../SlaveModules/ \

DEFINES +=  DEVICECONTROL_FUNCTIONMODULEPRIVATE_H \
            DEVICECONTROL_BASEMODULEPRIVATE_H \
            DEVICECONTROL_STEPPERMOTORPRIVATE_H \
            DEVICECONTROL_DIGITALINPUTPRIVATE_H \
            DEVICECONTROL_DIGITALOUTPUTPRIVATE_H \
            DEVICECONTROL_ANALOGINPUTPRIVATE_H \
            DEVICECONTROL_TEMPERATURECONTROLPRIVATE_H \
            DEVICECONTROL_RFID11785PRIVATE_H \
            DEVICEPROCESSING_H

DEFINES +=  Q_UNIT_TEST

HEADERS +=  ../../Include/Devices/DeviceState.h \
            ../../Include/Devices/DeviceBase.h \
            ../../Include/Devices/FmStepperInit.h \
            ../../Include/Devices/FmStepperMove.h \
            ../../Include/Devices/MoveOpenClosed.h \
            ../../Include/Devices/SetDrawerLed.h \
            ../../Include/Devices/DeviceXyz.h \
            ../../Include/Devices/ReadRackRfid.h \
            ../SlaveModules/*.h \
    ../../Include/Devices/MoveXYZ.h \
    ../../Include/Devices/MoveAck.h

SOURCES +=  \
            ../../Source/Devices/DeviceState.cpp \
            ../../Source/Devices/DeviceBase.cpp \
            ../../Source/Devices/DeviceXyz.cpp \
            ../../Source/Devices/FmStepperInit.cpp \
            ../../Source/Devices/FmStepperMove.cpp \
            ../../Source/Devices/MoveOpenClosed.cpp \
            ../../Source/Devices/SetDrawerLed.cpp \
            ../../Source/Devices/ReadRackRfid.cpp \
            ../SlaveModules/*.cpp \
    TestXyz.cpp \
    ../../Source/Devices/MoveXYZ.cpp \
    ../../Source/Devices/MoveAck.cpp

OTHER_FILES +=
