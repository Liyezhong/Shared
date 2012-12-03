!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}

# we create applications

TARGET = utTestBaseLoggerReusable

SOURCES +=  TestBaseLoggerReusable.cpp

HEADERS +=  DirectoryHelper.h


UseLibs(DataLogging DataManagement Global)




