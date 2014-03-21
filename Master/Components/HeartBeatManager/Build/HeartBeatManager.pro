!include("HeartBeatManager.pri") {
    error("HeartBeatManager.pri not found")
}

INCLUDEPATH += ../../../../Common/Components

TARGET = HeartBeatManager

HEADERS +=  ../Include/*.h \
    #../Include/HeartBeatManagerThreadController.h

SOURCES +=  ../Source/*.cpp
