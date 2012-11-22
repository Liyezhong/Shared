!include("DeviceControl.pri"):error("DeviceControl.pri not found")
QT += xml \
    xmlpatterns \
    network
INCLUDEPATH += ../../../../Common/Components
TARGET = utTestDeviceControl
HEADERS += *.h
SOURCES += *.cpp

UseLibs(Global DeviceControl)
