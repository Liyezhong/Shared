!include("SWUpdateManager.pri") {
    error("SWUpdateManager.pri not found")
}

TARGET = SWUpdateManager

HEADERS +=  ../Include/*.h

SOURCES += ../Source/*.cpp
