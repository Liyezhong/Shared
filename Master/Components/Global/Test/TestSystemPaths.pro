!include("Global.pri") {
    error("Global.pri not found")
}

TARGET = utTestSystemPaths

SOURCES += TestSystemPaths.cpp

UseLibs(Global)
