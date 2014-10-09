!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TARGET = utTestEventScenarioErrorXMLInfo

SOURCES += TestEventScenarioErrorXMLInfo.cpp

UseLibs(Global  DataManager DataLogging EventHandler)
