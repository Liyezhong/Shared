!include("EventHandler.pri") {
    error("EventHandler.pri not found")
}

TARGET = EventHandler

HEADERS +=  ../Include/*.h \
    ../../Global/Include/AlarmHandler.h

SOURCES +=  ../Source/*.cpp \
    ../../Global/Source/AlarmHandler.cpp
