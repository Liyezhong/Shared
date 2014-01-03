!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TARGET = utTestEventCSVInfo

SOURCES += TestEventCSVInfo.cpp

UseLibs(Global DataLogging EventHandler)
