!include("NetworkComponentsTest.pri") {
    error("NetworkComponentsTest.pri not found")
}

QT += xml \
      xmlpatterns \
      network

TARGET = utTestProtocolRxCommand

HEADERS += TestProtocolRxCommand.h

SOURCES += TestProtocolRxCommand.cpp


UseLibs(Global DataLogging NetworkComponents)

