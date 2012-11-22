!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}

# we create applications
TEMPLATE = app
TARGET = utTestComponentTestLoggerConfig

SOURCES += TestComponentTestLoggerConfig.cpp


UseLibs(DataLogging DataManagement Global)

