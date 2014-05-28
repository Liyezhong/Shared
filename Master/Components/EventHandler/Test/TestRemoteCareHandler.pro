!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TARGET = utTestRemoteCareHandler
# we include current directory
INCLUDEPATH +=  ..

SOURCES = TestRemoteCareHandler.cpp


UseLibs(NetCommands Global DataLogging EventHandler)
