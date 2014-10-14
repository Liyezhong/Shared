!include("DataManager.pri"):error("DataManager.pri not found")

QT += xml
QT += xmlpatterns
QT += network

TARGET = utTestDeviceConfigurationSepia

INCLUDEPATH += ../../../ \
 ../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../
RESOURCES += ../Containers/DeviceConfiguration/Resources/*.qrc

SOURCES = TestDeviceConfigurationSepia.cpp

UseDepLibs(Global DataLogging Threads NetCommands DeviceControl DataManager RemoteCareManager \
		   RemoteCareController HeartBeatManager EventHandler GPIOManager ExternalProcessController \
                   NetworkComponents StateMachines PasswordManager SWUpdateManager ExportController EncryptionDecryption)

LIBS += -ldl
