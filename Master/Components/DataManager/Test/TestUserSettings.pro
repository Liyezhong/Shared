!include("DataManager.pri"):error("DataManager.pri not found")

TARGET = utTestUserSettings

INCLUDEPATH += ../../../ \
 ../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../
RESOURCES += ../Containers/UserSettings/Resources/*.qrc


SOURCES = TestUserSettings.cpp

UseDepLibs(Global DataLogging Threads NetCommands DeviceControl DataManager RemoteCareManager \
		   RemoteCareController HeartBeatManager EventHandler GPIOManager ExternalProcessController \
		   NetworkComponents StateMachines PasswordManager SWUpdateManager ExportController ImportExport)

LIBS += -ldl
