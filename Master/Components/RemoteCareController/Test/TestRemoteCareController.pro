!include("TestRemoteCareController.pri"){error("TestRemoteCareController.pri not found")}

TARGET = utTestRemoteCareController
SOURCES += TestRemoteCareController.cpp

QT += xml
QT += xmlpatterns
QT += network

INCLUDEPATH += ../../
INCLUDEPATH += ../../../Components/
DEPENDPATH += ../../

UseLibs(DataLogging DataManager EventHandler ExternalProcessController Global NetworkComponents StateMachines RemoteCareController RemoteCareManager NetCommands Threads GPIOManager HeartBeatManager)

LIBS += -ldl
