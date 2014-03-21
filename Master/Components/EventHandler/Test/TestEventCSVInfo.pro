!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TARGET = TestEventCSVInfo

SOURCES += TestEventCSVInfo.cpp

UseLibs(Global DataLogging EventHandler)
