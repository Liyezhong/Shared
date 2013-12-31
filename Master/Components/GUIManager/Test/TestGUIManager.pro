!include("GUIManager.pri"):error("GUIManager.pri not found")

TARGET = utTestGUIManager
SOURCES += TestGUIManager.cpp

INCLUDEPATH += ../../

DEPENDPATH += ../../


UseLibs(ExternalProcessController DataManager GUIManager GPIOManager HeartBeatManager PasswordManager RemoteCareManager RemoteCareController NetCommands Threads DataLogging Global NetworkComponents StateMachines EventHandler ExportController)
