!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TARGET = utTestHimalayaEventHandlerThreadController

SOURCES += TestHimalayaEventHandlerThreadController.cpp

INCLUDEPATH += ../..
#HEADERS +=  ../Include/Main.h \
#            ../../../Include/ColoradoEventCodes.h \
#            ../../../Include/DataLoggingSources.h \
#            ../../../Include/ColoradoProcessExitCodes.h

UseLibs(Global DataLogging EventHandler Threads NetCommands Global)
QT += network