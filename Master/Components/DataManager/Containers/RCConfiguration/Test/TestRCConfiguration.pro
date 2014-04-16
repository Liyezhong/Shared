!include("../../../Test/DataManager.pri"):error("DataManager.pri not found")

TARGET = utTestRCConfiguration
SOURCES += TestRCConfiguration.cpp

INCLUDEPATH += ../../../../ \
 ../../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../../
RESOURCES += ../Resources/*.qrc \
    ../Resources/RCConfigurationXml.qrc
	
UseLib(../../../../DataManager ../../../../Global)
