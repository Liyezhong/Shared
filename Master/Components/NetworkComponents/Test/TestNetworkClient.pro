!include("NetworkComponentsTest.pri") {
    error("NetworkComponentsTest.pri not found")
}

QT += xml \
      xmlpatterns \
      network

TARGET = utTestNetworkClient

HEADERS += TestNetworkClient.h

SOURCES += TestNetworkClient.cpp

UseLibs(Global DataLogging NetworkComponents)

