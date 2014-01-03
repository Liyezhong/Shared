!include("../../../Test/DataManager.pri"):error("DataManager.pri not found")

TARGET = utTestDeviceConfigurationSepia
SOURCES += TestDeviceConfigurationSepia.cpp

INCLUDEPATH += ../../../../ \
 ../../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../../
RESOURCES += ../Resources/*.qrc \
	
UseLib(../../../../DataManager ../../../../Global)
