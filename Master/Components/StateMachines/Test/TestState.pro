!include("StateMachines.pri") {
    error("StateMachines.pri not found")
}

TARGET = utTestState

HEADERS += TestState.h \
           DerivedState.h

SOURCES += TestState.cpp \
           DerivedState.cpp

UseLibs(Global DataLogging StateMachines EventHandler)
