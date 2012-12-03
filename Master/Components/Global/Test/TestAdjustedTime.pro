!include("Global.pri") {
    error("Global.pri not found")
}

TARGET = utTestAdjustedTime

SOURCES += TestAdjustedTime.cpp

UseLibs(Global)
