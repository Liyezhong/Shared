!include("DataManager.pri"):error("DataManager.pri not found")

QT += xml
QT += xmlpatterns
QT += network

TARGET = utTestRCConfiguration

INCLUDEPATH += ../../../ \
 ../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../
RESOURCES += ../Containers/RCConfiguration/Resources/*.qrc


SOURCES = TestRCConfiguration.cpp

UseDepLibs(Global DataLogging Threads NetCommands DeviceControl DataManager RemoteCareManager \
		   RemoteCareController HeartBeatManager EventHandler GPIOManager ExternalProcessController \
		   NetworkComponents StateMachines PasswordManager SWUpdateManager ExportController ImportExport)

LIBS += -ldl
