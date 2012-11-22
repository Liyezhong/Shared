!include("../../../Test/PlatformExport.pri") {
    error("../../../Test/PlatformExport.pri not found")
}

TARGET = utTestExportData
SOURCES += TestExportData.cpp

INCLUDEPATH += ../../../../ \
 ../../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../../
	
UseLibs(ExportData)
UseLibsShared(DataManager)
UseLibsPlatform(Global ImportExport)
