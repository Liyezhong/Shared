!include("TestExternalProcessController.pri") {
    error("TestExternalProcessController.pri not found")
}

TARGET = ExternalProcessController

HEADERS +=  ../Include/*.h \
            ../Include/ExternalProcessStates/*.h

SOURCES +=  ../Source/*.cpp \
            ../Source/ExternalProcessStates/*.cpp

INCLUDEPATH += ../../../../../Himalaya/HimalayaMain/Gui/Test/Components/
