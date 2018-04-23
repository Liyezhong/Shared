!include("DeviceControl.pri"):error("DeviceControl.pri not found")

QT += xml
QT += xmlpatterns
QT += network


TARGET = utTestIDeviceProcessing

INCLUDEPATH += ../..
INCLUDEPATH += ../Include
INCLUDEPATH += ../../../../Common/Components/ \
               ../hwconfig/ \
               ../../../../../Shared/ExternalPackages/xsde/libxsde

SOURCES += TestIDeviceProcessing.cpp

HEADERS += ../Include/Interface/*.h
HEADERS += ../Include/Global/*.h


#LIBS += -L$$PWD/../hwconfig/libxsde/xsde/libxsde.a
#PRE_TARGETDEPS += ../hwconfig/libxsde/xsde/libxsde.a



UseLibs(Global DataManager DeviceControl)

LIBS += ../../../../../Shared/ExternalPackages/xsde/libxsde/xsde/libxsde.a

#unix:!macx: LIBS += -L../../../../../Shared/ExternalPackages/xsde/libxsde/xsde/ -lxsde

#INCLUDEPATH += $../../../../../Shared/ExternalPackages/xsde/libxsde
#DEPENDPATH += $../../../../../Shared/ExternalPackages/xsde/libxsde

#unix:!macx: PRE_TARGETDEPS += ../../../../../Shared/ExternalPackages/xsde/libxsde/xsde/libxsde.a
