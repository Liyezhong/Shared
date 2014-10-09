!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TARGET = utTestEventHandlerThreadController

SOURCES += TestEventHandlerThreadController.cpp

INCLUDEPATH += ../..
#HEADERS +=  ../Include/Main.h \
#            ../../../Include/ColoradoEventCodes.h \
#            ../../../Include/DataLoggingSources.h \
#            ../../../Include/ColoradoProcessExitCodes.h

UseLibs(Global  DataManager DataLogging EventHandler Threads NetCommands Global)

QT += network
