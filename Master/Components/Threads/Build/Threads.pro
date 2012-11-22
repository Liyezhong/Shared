!include("Threads.pri") {
    error("Threads.pri not found")
}

INCLUDEPATH += ../../../../Common/Components

TARGET = Threads

HEADERS +=  ../Include/*.h

SOURCES +=  ../Source/*.cpp
