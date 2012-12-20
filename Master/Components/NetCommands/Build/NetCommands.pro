!include("NetCommands.pri") {
    error("NetCommands.pri not found")
}

TARGET = NetCommands

HEADERS +=  ../Include/*.h \
    ../Include/CmdExportDayRunLogRequest.h \
    ../Include/CmdExportDayRunLogReply.h

SOURCES +=  ../Source/*.cpp \
    ../Source/CmdExportDayRunLogRequest.cpp \
    ../Source/CmdExportDayRunLogReply.cpp
