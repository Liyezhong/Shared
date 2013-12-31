!include("GPIOManager.pri") {
    error("GPIOManager.pri not found")
}

INCLUDEPATH += ../../../../Common/Components

TARGET = GPIOManager

HEADERS +=  ../Include/*.h \
            ../Commands/Include/*.h

SOURCES +=  ../Source/*.cpp \
            ../Commands/Source/*.cpp

