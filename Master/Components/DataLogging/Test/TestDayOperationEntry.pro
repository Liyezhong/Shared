!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}

# we create applications
TEMPLATE = app
TARGET = utTestDayOperationEntry

SOURCES += TestDayOperationEntry.cpp


UseLibs(DataLogging DataManagement Global)

