!include("ExportController.pri"):error("ExportController.pri not found")

TARGET = utTestExportController
SOURCES += TestExportController.cpp


QT += xml
QT += xmlpatterns
QT += network

INCLUDEPATH += ../../

DEPENDPATH += ../../


UseLibs(ExternalProcessController Threads DataLogging Global NetworkComponents StateMachines EventHandler ExportController)

QT += network
QT += xml
QT += xmlpatterns
