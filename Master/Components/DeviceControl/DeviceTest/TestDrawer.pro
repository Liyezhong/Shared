#!include("device.pri"):error("device.pri not found")

INCLUDEPATH +=	../../../Components/ \
               	../../../../Common/Components/

CONFIG += qtestlib debug

QT += 	xml \
        xmlpatterns
QT -= 	gui

TARGET = utDrawer

HEADERS += *.h
SOURCES += Device.cpp \
           Drawer.cpp \
           DrawerLed.cpp \
           DrawerMotion.cpp \
           ReadRackRfid.cpp \
           FmStepperMotorInitialize.cpp \
           State.cpp \
           HoodDevice.cpp \
           TestDrawer.cpp
