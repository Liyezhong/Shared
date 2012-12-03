!include("../../../Test/DataManager.pri"):error("DataManager.pri not found")

TARGET = utTestExportConfiguration
SOURCES += TestExportConfiguration.cpp

INCLUDEPATH += ../../../../ \
 ../../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../../
RESOURCES += ../Resources/*.qrc
	
UseLib(../../../../DataManager ../../../../Global)
