!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TARGET = utTestEventXMLInfo

SOURCES += TestEventXMLInfo.cpp

UseLibs(Global  DataManager DataLogging EventHandler)
