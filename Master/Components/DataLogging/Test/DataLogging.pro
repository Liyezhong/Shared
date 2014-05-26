!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}

TEMPLATE = subdirs

SUBDIRS     =   TestBaseLogger.pro
SUBDIRS  +=   TestBaseLoggerReusable.pro
SUBDIRS  +=   TestDayOperationLoggerConfig.pro
SUBDIRS  +=   TestDayOperationEntry.pro
SUBDIRS  +=   TestEventFilter.pro
SUBDIRS  +=   TestDayLogFileInformation.pro
SUBDIRS  +=   TestEventFilterNetworkServer.pro

CONFIG += ordered
