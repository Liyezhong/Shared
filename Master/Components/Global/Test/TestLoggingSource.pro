!include("Global.pri") {
    error("Global.pri not found")
}

TARGET = utTestLoggingSource

SOURCES += TestLoggingSource.cpp

UseLibs(Global)
