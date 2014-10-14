!include("DataManager.pri"):error("DataManager.pri not found")

TARGET = utTestXmlConfigFile

INCLUDEPATH += ../../../ \
 ../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../

SOURCES = TestXmlConfigFile.cpp

UseDepLibs(Global DataLogging Threads NetCommands DeviceControl DataManager RemoteCareManager \
		   RemoteCareController HeartBeatManager EventHandler GPIOManager ExternalProcessController \
                   NetworkComponents StateMachines PasswordManager SWUpdateManager ExportController EncryptionDecryption)

LIBS += -ldl
