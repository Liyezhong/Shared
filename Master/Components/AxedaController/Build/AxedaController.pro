!include("AxedaController.pri") {
    error("AxedaController.pri not found")
}

INCLUDEPATH += ../../../../Common/Components

TARGET = AxedaController

HEADERS +=  ../Include/*.h \
            ../../../Components/RemoteCareAgent/Include/Commands/*.h

SOURCES +=  ../Source/*.cpp \
            ../../../Components/RemoteCareAgent/Source/Commands/*.cpp
