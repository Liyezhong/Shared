!include("Global.pri") {
    error("Global.pri not found")
}

TARGET = utTestSharedPointer

SOURCES += TestSharedPointer.cpp

UseLibs(Global)
