!include("StateMachines.pri") {
    error("StateMachines.pri not found")
}

TARGET = utTestStateEngine

HEADERS += TestStateEngine.h \
           DerivedState.h

SOURCES += TestStateEngine.cpp \
           DerivedState.cpp

UseLibs(Global DataLogging StateMachines EventHandler)
