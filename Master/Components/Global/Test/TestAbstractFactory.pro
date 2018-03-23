!include("Global.pri") {
    error("Global.pri not found")
}

TARGET = utTestAbstractFactory

HEADERS += TestAbstractFactory.h

SOURCES += TestAbstractFactory.cpp

UseDepLibs(Global)
