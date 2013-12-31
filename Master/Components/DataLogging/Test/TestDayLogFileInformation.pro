!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}

# we create applications
TEMPLATE = app
TARGET = utTestDayLogFileInformation

SOURCES += TestDayLogFileInformation.cpp


UseLibs(DataLogging EventHandler Global)

