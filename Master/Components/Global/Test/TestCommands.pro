!include("Global.pri") {
    error("Global.pri not found")
}

TARGET = utTestCommands

SOURCES += TestCommands.cpp

UseLibs(Global)
