!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TARGET = TestActionHandler

SOURCES += TestActionHandler.cpp \


UseLibs(NetCommands Global DataLogging EventHandler)
