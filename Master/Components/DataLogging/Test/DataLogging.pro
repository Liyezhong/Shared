!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}

TEMPLATE = subdirs

SUBDIRS =   TestBaseLogger.pro \
            TestBaseLoggerReusable.pro \
            TestDayOperationLoggerConfig.pro \
            TestDayOperationEntry.pro \
            TestEventFilter.pro \
            TestLoggingObject.pro

CONFIG += ordered
