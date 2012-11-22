!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}

TARGET = DataLogging

HEADERS +=  ../Include/*.h \
            ../Include/Commands/*.h

SOURCES +=  ../Source/*.cpp \
            ../Source/Commands/*.cpp
