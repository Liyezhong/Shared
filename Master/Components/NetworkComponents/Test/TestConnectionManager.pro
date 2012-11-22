!include("NetworkComponentsTest.pri") {
    error("NetworkComponentsTest.pri not found")
}

QT += xml \
      xmlpatterns \
      network

TARGET = utTestConnectionManager

HEADERS += TestConnectionManager.h

SOURCES += TestConnectionManager.cpp


UseLibs(Global DataLogging NetworkComponents)

