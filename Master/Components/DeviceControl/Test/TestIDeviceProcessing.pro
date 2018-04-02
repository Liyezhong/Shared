!include("DeviceControl.pri"):error("DeviceControl.pri not found")

QT += xml
QT += xmlpatterns
QT += network


TARGET = utTestIDeviceProcessing

INCLUDEPATH += ../..
INCLUDEPATH += ../Include
INCLUDEPATH += ../../../../Common/Components/ \
               ../hwconfig/ \
               ../hwconfig/libxsde

SOURCES += TestIDeviceProcessing.cpp

HEADERS += ../Include/Interface/*.h
HEADERS += ../Include/Global/*.h

#LIBS += -L$$PWD/../hwconfig/libxsde/xsde/libxsde.a
#PRE_TARGETDEPS += ../hwconfig/libxsde/xsde/libxsde.a


UseLibs(Global DataManager DeviceControl)

unix:!macx: LIBS += -L$$PWD/../hwconfig/libxsde/xsde/ -lxsde

INCLUDEPATH += $$PWD/../hwconfig/libxsde/xsde
DEPENDPATH += $$PWD/../hwconfig/libxsde/xsde

unix:!macx: PRE_TARGETDEPS += $$PWD/../hwconfig/libxsde/xsde/libxsde.a
