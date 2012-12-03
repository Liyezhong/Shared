!include("../../../Test/DataManager.pri"):error("DataManager.pri not found")

TARGET = utTestSWVersionList
SOURCES += TestSWVersionList.cpp

INCLUDEPATH += ../../../../ \
 ../../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../../
RESOURCES += ../Resources/*.qrc
	
UseLib(../../../../DataManager ../../../../Global)
