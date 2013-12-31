!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}

TARGET = DataLogging

HEADERS +=  ../Include/*.h \

SOURCES +=  ../Source/*.cpp \
