!include("NetworkComponentsTest.pri") {
    error("NetworkComponentsTest.pri not found")
}

QT += xml \
      xmlpatterns \
      network

TARGET = utTestProtocolTxCommand

HEADERS += TestProtocolTxCommand.h

SOURCES += TestProtocolTxCommand.cpp


UseLibs(Global DataLogging NetworkComponents)

