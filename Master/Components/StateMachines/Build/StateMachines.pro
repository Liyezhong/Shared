!include("StateMachines.pri") {
    error("StateMachines.pri not found")
}

TARGET = StateMachines

HEADERS +=  ../Include/*.h

SOURCES +=  ../Source/*.cpp
