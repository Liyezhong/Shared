!include("Global.pri") {
    error("Global.pri not found")
}

TARGET = utTestUtils

SOURCES += TestUtils.cpp

UseLibs(Global)
