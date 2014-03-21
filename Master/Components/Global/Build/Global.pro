!include("Global.pri") {
    error("Global.pri not found")
}

TARGET = Global

HEADERS +=  ../Include/*.h \
            ../Include/Commands/*.h

SOURCES +=  ../Source/*.cpp \
            ../Source/Commands/*.cpp
