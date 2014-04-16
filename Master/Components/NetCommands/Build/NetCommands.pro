!include("NetCommands.pri") {
    error("NetCommands.pri not found")
}

TARGET = NetCommands

HEADERS +=  ../Include/*.h
SOURCES +=  ../Source/*.cpp
