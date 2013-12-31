#-------------------------------------------------
#
# Project created by QtCreator 2013-01-09T17:16:51
#
#-------------------------------------------------

!include("../../Test/DataManager.pri"):error("DataManager.pri not found")

TARGET = utTestCmdInterface

INCLUDEPATH += ../../../ \
 ../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../ \

UseLib(../../../DataManager ../../../ExternalProcessController ../../../HeartBeatManager ../../../Threads ../../../Global ../../../EventHandler ../../../DataLogging ../../../GPIOManager ../../../NetCommands ../../../RemoteCareManager  ../../../RemoteCareController ../../../StateMachines ../../../NetworkComponents ../../../DeviceControl)

SOURCES += \
    TestCmdInterface.cpp

