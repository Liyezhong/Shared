!include("DataManager.pri"):error("DataManager.pri not found")

QT += xml
QT += xmlpatterns
QT += network

TARGET = utTestDeviceConfiguration

INCLUDEPATH += ../../../ \
 ../../../../../../../Platform/Master/Components/

RESOURCES += ../Containers/DeviceConfiguration/Resources/*.qrc

DEPENDPATH += ../../../ \

SOURCES = TestDeviceConfiguration.cpp

UseDepLibs(Global DataLogging Threads NetCommands DeviceControl DataManager RemoteCareManager \
		   RemoteCareController HeartBeatManager EventHandler GPIOManager ExternalProcessController \
                   NetworkComponents StateMachines PasswordManager SWUpdateManager ExportController EncryptionDecryption)

LIBS += -ldl
