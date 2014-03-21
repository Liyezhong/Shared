!include("../../../Test/DataManager.pri"):error("DataManager.pri not found")

TARGET = utTestUserSettings
SOURCES += TestUserSettings.cpp

INCLUDEPATH += ../../../../ \
 ../../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../../
RESOURCES += ../Resources/*.qrc \
	
UseLib(../../../../DataManager ../../../../Global)
