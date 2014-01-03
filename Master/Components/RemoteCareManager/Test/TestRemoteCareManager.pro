!include("TestRemoteCareManager.pri"){error("TestRemoteCareManager.pri not found")}

TARGET = utTestRemoteCareManager
SOURCES += TestRemoteCareManager.cpp

INCLUDEPATH += ../../
DEPENDPATH += ../../

UseLibs(DataLogging DataManager EventHandler ExternalProcessController Global NetworkComponents StateMachines RemoteCareController RemoteCareManager NetCommands Threads GPIOManager HeartBeatManager)

RESOURCES += \
    ../Resources/Resources.qrc

