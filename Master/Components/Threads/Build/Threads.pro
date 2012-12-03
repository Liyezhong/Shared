!include("Threads.pri") {
    error("Threads.pri not found")
}



TARGET = Threads
INCLUDEPATH += ../../../../Common/Components

HEADERS +=  ../Include/*.h

SOURCES +=  ../Source/*.cpp
