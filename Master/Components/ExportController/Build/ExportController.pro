!include("ExportController.pri") {
    error("ExportController.pri not found")
}

TARGET = ExportController

HEADERS +=  ../Include/*.h

SOURCES +=  ../Source/*.cpp
