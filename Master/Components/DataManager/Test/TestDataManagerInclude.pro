!include("DataManager.pri"):error("DataManager.pri not found")

QT += xml
QT += xmlpatterns
QT += network

TARGET = utTestDataManagerInclude

INCLUDEPATH += ../../../
INCLUDEPATH += ../../../../../Himalaya/HimalayaMain/Master/Components/
INCLUDEPATH += ../../../../../Himalaya/Shared/Master/Components/
INCLUDEPATH += ../../../../../Himalaya/HimalayaMain/Master/Include

DEPENDPATH += ../../../

SOURCES = TestDataManagerInclude.cpp

UseDepLibs(Global DataLogging Threads NetCommands DeviceControl DataManager RemoteCareManager \
		   RemoteCareController HeartBeatManager EventHandler GPIOManager ExternalProcessController \
                   NetworkComponents StateMachines PasswordManager SWUpdateManager ExportController EncryptionDecryption)

LIBS += -ldl
