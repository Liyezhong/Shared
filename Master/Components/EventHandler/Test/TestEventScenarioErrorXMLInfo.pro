!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TARGET = TestEventScenarioErrorXMLInfo

SOURCES += TestEventScenarioErrorXMLInfo.cpp

UseLibs(Global DataLogging EventHandler)

HEADERS +=
