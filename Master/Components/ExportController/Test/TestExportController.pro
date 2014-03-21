!include("ExportController.pri"):error("ExportController.pri not found")

TARGET = utTestExportController
SOURCES += TestExportController.cpp

INCLUDEPATH += ../../

DEPENDPATH += ../../


UseLibs(ExternalProcessController Threads DataLogging Global NetworkComponents StateMachines EventHandler ExportController)
