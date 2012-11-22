!include("NetworkComponentsTest.pri") {
    error("NetworkComponentsTest.pri not found")
}

QT += xml \
      xmlpatterns \
      network

TARGET = utTestNetworkClientDevice

HEADERS += TestNetworkClientDevice.h

SOURCES += TestNetworkClientDevice.cpp


UseLibs(Global DataLogging NetworkComponents)

