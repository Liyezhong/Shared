!include("HeartBeatManager.pri") {
    error("HeartBeatManager.pri not found")
}

INCLUDEPATH += ../../../../Common/Components

TARGET = HeartBeatManager

HEADERS +=  ../Include/*.h \
            ../Include/Commands/*.h

SOURCES +=  ../Source/*.cpp \
            ../Source/Commands/*.cpp
