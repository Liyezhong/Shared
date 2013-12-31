!include("TestRemoteCareController.pri"){error("TestRemoteCareController.pri not found")}

TARGET = utTestRemoteCareController
SOURCES += TestRemoteCareController.cpp

INCLUDEPATH += ../../
DEPENDPATH += ../../

UseLibs(DataLogging DataManager EventHandler ExternalProcessController Global NetworkComponents StateMachines RemoteCareController RemoteCareManager NetCommands Threads GPIOManager HeartBeatManager)

