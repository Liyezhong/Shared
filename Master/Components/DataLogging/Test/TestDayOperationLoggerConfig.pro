!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}

# we create applications
TEMPLATE = app
TARGET = utTestDayOperationLoggerConfig

SOURCES += TestDayOperationLoggerConfig.cpp


UseLibs(DataLogging EventHandler Global)

