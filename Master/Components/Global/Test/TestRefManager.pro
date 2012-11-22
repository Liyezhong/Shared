!include("Global.pri") {
    error("Global.pri not found")
}

TARGET = utTestRefManager

SOURCES += TestRefManager.cpp

UseLibs(Global)
