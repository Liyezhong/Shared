!include("GUIManager.pri") {
    error("GUIManager.pri not found")
}

TARGET = GUIManager

HEADERS +=  ../Include/*.h

SOURCES +=  ../Source/*.cpp
