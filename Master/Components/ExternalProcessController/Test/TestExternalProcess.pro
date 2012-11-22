!include("ExternalProcessTest.pri") {
    error("ExternalProcessTest.pri not found")
}

TARGET = utTestExternalProcess

HEADERS += TestExternalProcess.h

SOURCES += TestExternalProcess.cpp


UseLibs(ExternalProcessController Global)
