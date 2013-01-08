!include("NetCommands.pri") {
    error("NetCommands.pri not found")
}

TARGET = NetCommands

HEADERS +=  ../Include/*.h \
    ../Include/CmdRequestSepiaRack.h \
    ../Include/CmdRequestSepiaParameterSet.h
SOURCES +=  ../Source/*.cpp \
    ../Source/CmdRequestSepiaRack.cpp \
    ../Source/CmdRequestSepiaParameterSet.cpp

