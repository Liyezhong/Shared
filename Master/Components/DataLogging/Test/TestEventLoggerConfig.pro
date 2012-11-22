!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}

# we create applications
TEMPLATE = app
TARGET = utTestEventLoggerConfig

SOURCES += TestEventLoggerConfig.cpp


UseLibs(DataLogging DataManagement Global)

