!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}

# we create applications
TEMPLATE = app
TARGET = utTestEventEntry

SOURCES += TestEventEntry.cpp


UseLibs(DataLogging DataManagement Global)

