!include("HeartBeatManager.pri"){error("HeartBeatManager.pri not found")}

TARGET = utTestHeartBeatThread
SOURCES += TestHeartBeatThread.cpp

INCLUDEPATH += ../../
DEPENDPATH += ../../

UseLibs(HeartBeatManager Threads Global )

