!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}

QT += network

TEMPLATE = app
TARGET = utTestEventFilterNetworkServer

SOURCES += TestEventFilterNetworkServer.cpp

UseLibs(DataLogging EventHandler Global)

