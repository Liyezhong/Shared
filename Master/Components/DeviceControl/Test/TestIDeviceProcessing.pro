!include("DeviceControl.pri"):error("DeviceControl.pri not found")

QT += xml
QT += xmlpatterns
QT += network


TARGET = utTestIDeviceProcessing

INCLUDEPATH += ../..
INCLUDEPATH += ../Include
INCLUDEPATH += ../../../../Common/Components/

SOURCES += TestIDeviceProcessing.cpp

HEADERS += ../Include/Interface/*.h
HEADERS += ../Include/Global/*.h

UseLibs(Global DeviceControl)
