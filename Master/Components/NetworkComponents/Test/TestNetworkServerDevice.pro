!include("NetworkComponentsTest.pri") {
    error("NetworkComponentsTest.pri not found")
}

QT += xml \
      xmlpatterns \
      network

TARGET = utTestNetworkServerDevice

HEADERS += TestNetworkServerDevice.h \
           DerivedRxCommand.h

SOURCES += TestNetworkServerDevice.cpp \
           DerivedRxCommand.cpp


UseLibs(Global DataLogging NetworkComponents)

