!include("RemoteCareManager.pri") {
    error("RemoteCareManager.pri not found")
}

TARGET = RemoteCareManager

#include Platform
INCLUDEPATH += ../../../../../../Platform

HEADERS +=  ../Include/*.h

SOURCES += ../Source/*.cpp
