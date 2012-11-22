!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}

# we create applications
TEMPLATE = app
TARGET = utTestComponentTestEntry

SOURCES += TestComponentTestEntry.cpp

UseLibs(DataLogging DataManagement Global)

