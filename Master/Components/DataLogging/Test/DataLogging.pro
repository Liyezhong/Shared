!include("DataLogging.pri") {
    error("DataLogging.pri not found")
}

TEMPLATE = subdirs

SUBDIRS =   TestBaseLogger.pro \
            TestBaseLoggerReusable.pro \
            TestComponentTestEntry.pro \
            TestComponentTestLoggerConfig.pro \
            TestDayOperationLoggerConfig.pro \
            TestDayOperationEntry.pro \
            TestEventEntry.pro \
            TestEventFilter.pro \
            TestEventLoggerConfig.pro \
            TestLoggingObject.pro

CONFIG += ordered
