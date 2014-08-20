!include("../../TestKeyBoard.pri"):error("TestKeyBoard.pri not found")
TARGET = utKeyBoard
SOURCES += TestKeyBoard.cpp


INCLUDEPATH += ../../ \
            ../../../../../../../Platform/Master/Components \

DEPENDPATH += ../../

UseLibs(KeyBoard MainMenu Application)
UseLibsPlatform(Global)
