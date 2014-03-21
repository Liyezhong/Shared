!include("../../../Test/Export.pri") {
    error("../../../Test/Export.pri not found")
}

TARGET = utTestMain
SOURCES += TestMain.cpp

INCLUDEPATH += ../../ \
 ../../../../Master/Components/

DEPENDPATH += ../../

