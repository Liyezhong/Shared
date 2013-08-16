!include("../../../Test/Himalaya.pri"):error("Himalaya.pri not found")

TARGET = utTestExportController
SOURCES += TestExportController.cpp

INCLUDEPATH += ../../../../ \
 ../../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../../
	
UseLibs(ExportController)
UseLibsPlatform(ExternalProcessController Threads DataLogging Global NetworkComponents StateMachines EventHandler)
