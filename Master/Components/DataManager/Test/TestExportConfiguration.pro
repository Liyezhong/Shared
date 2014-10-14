!include("DataManager.pri"):error("DataManager.pri not found")

QT += xml
QT += xmlpatterns
QT += network

TARGET = utTestExportConfiguration

INCLUDEPATH += ../../../ \
 ../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../
RESOURCES += ../Containers/ExportConfiguration/Resources/*.qrc


SOURCES = TestExportConfiguration.cpp

UseDepLibs(Global DataLogging Threads NetCommands DeviceControl DataManager RemoteCareManager \
		   RemoteCareController HeartBeatManager EventHandler GPIOManager ExternalProcessController \
                   NetworkComponents StateMachines PasswordManager SWUpdateManager ExportController EncryptionDecryption)

LIBS += -ldl
