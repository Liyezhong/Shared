!include("../../../Test/DataManager.pri"):error("DataManager.pri not found")

TARGET = utTestDeviceConfiguration
SOURCES += TestDeviceConfiguration.cpp

INCLUDEPATH += ../../../../ \
 ../../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../../
RESOURCES += ../Resources/*.qrc \
	
UseLib(../../../../DataManager ../../../../Global)
