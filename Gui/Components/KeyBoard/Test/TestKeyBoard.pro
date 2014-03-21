!include("../../../Test/Platform.pri"):error("TestDataManager.pri not found")
TARGET = utKeyBoard
SOURCES += TestKeyBoard.cpp

INCLUDEPATH += ../../

DEPENDPATH += ../../

UseLibs(KeyBoard MainMenu Application)
UseLib(../../../../Master/Components/Global)
