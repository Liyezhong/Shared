!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TARGET = EventHandler

HEADERS +=  ../Include/*.h

SOURCES +=  ../Source/*.cpp
