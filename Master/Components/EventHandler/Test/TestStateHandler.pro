!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TARGET = utTestStateHandler

SOURCES += TestStateHandler.cpp

UseLibs(NetCommands DataManager Global DataLogging EventHandler)
