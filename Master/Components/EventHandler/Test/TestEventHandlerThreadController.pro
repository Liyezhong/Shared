!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TARGET = TestEventHandlerThreadController

SOURCES += TestEventHandlerThreadController.cpp

INCLUDEPATH += ../..
HEADERS +=

UseLibs(Global DataLogging EventHandler Threads NetCommands Global)

QT += network \
      xml\
      QTest
