!include("ExternalProcessController.pri") {
    error("ExternalProcessController.pri not found")
}

TARGET = ExternalProcessController

HEADERS +=  ../Include/*.h \
            ../Include/ExternalProcessStates/*.h

SOURCES +=  ../Source/*.cpp \
            ../Source/ExternalProcessStates/*.cpp

