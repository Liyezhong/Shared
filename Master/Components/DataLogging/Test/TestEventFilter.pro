!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}
# we create applications
TEMPLATE = app
TARGET = utTestEventFilter

SOURCES += TestEventFilter.cpp


UseLibs(DataLogging EventHandler Global)

