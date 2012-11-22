!include("DataManagement.pri") {
    error("DataManagement.pri not found")
}

TARGET = DataManagement

HEADERS +=  ../Include/*.h
SOURCES += ../Source/*.cpp
