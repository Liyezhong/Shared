!include("SoftSwitchManager.pri") {
    error("SoftSwitchManager.pri not found")
}

INCLUDEPATH += ../../../../Common/Components

TARGET = SoftSwitchManager

HEADERS +=  ../Include/*.h

SOURCES +=  ../Source/*.cpp
