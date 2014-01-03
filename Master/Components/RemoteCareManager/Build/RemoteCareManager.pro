!include("RemoteCareManager.pri") {
    error("RemoteCareManager.pri not found")
}

TARGET = RemoteCareManager

HEADERS +=  ../Include/*.h

SOURCES += ../Source/*.cpp
