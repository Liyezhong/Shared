!include("Global.pri") {
    error("Global.pri not found")
}

TARGET = utTestTranslator

SOURCES += TestTranslator.cpp

UseLibs(Global)
