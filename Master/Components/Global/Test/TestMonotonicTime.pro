!include("Global.pri") {
    error("Global.pri not found")
}

TARGET = utTestMonotonicTime

SOURCES += TestMonotonicTime.cpp

UseLibs(Global)
LIBS += -lrt
