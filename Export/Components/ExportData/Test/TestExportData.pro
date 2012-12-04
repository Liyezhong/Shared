!include("../../../Test/Export.pri") {
    error("../../../Test/Export.pri not found")
}

TARGET = utTestExportData
SOURCES += TestExportData.cpp

INCLUDEPATH += ../../ \
 ../../../../../Platform/Master/Components/

DEPENDPATH += ../../
	
UseLibs(ExportData)
UseLibsPlatform(Global ImportExport DataManager)
