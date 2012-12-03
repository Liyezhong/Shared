!include("NetworkComponentsTest.pri") {
    error("NetworkComponentsTest.pri not found")
}

QT += xml \
      xmlpatterns \
      network

TARGET = utTestNetworkServer

HEADERS += TestNetworkServer.h

SOURCES += TestNetworkServer.cpp


UseLibs(Global DataLogging NetworkComponents)

