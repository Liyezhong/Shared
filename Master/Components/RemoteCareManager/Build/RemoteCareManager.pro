!include("RemoteCareManager.pri") {
    error("RemoteCareManager.pri not found")
}

TARGET = RemoteCareManager

#include Platform
INCLUDEPATH += ../../../../../Shared

HEADERS +=  ../Include/*.h

SOURCES += ../Source/*.cpp
