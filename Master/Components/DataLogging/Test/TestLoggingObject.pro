!include("DataLogging.pri"):error("DataLogging.pri not found")

# we create applications
TEMPLATE = app
TARGET = utTestLoggingObject
SOURCES += TestLoggingObject.cpp

UseLibs(DataLogging DataManagement Global)

