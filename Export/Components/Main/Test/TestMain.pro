!include("../../../Test/PlatformExport.pri") {
    error("../../../Test/PlatformExport.pri not found")
}

TARGET = utTestMain
SOURCES += TestMain.cpp

INCLUDEPATH += ../../ \
 ../../../../Master/Components/

DEPENDPATH += ../../

