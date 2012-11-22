!include("Global.pri") {
    error("Global.pri not found")
}

TARGET = utTestException

SOURCES += TestException.cpp

UseLibs(Global)
