!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}



TARGET = utTestBaseLogger

SOURCES +=  TestBaseLogger.cpp \
            TestBaseLogger2.cpp

HEADERS +=  DirectoryHelper.h \
            TestBaseLogger.h

UseLibs(DataLogging EventHandler Global)


