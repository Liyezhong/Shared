!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TARGET = utTestActionHandler

SOURCES += TestActionHandler.cpp \


UseLibs(NetCommands Global DataLogging EventHandler)
