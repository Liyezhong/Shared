!include("Global.pri") {
    error("Global.pri not found")
}

TARGET = utTestTranslatableString

SOURCES += TestTranslatableString.cpp

UseLibs(Global)
