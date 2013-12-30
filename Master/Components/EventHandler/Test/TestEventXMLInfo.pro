!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TARGET = TestEventXMLInfo

SOURCES += TestEventXMLInfo.cpp

UseLibs(Global DataLogging EventHandler)

HEADERS +=
