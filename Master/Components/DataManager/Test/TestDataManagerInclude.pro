!include("DataManager.pri"):error("DataManager.pri not found")

QT += xml
QT += xmlpatterns
QT += network

TARGET = utTestDataManagerInclude

INCLUDEPATH += ../../../
INCLUDEPATH += ../../../../../Master/Components/
INCLUDEPATH += ../../../../../Shared/Common/Components/
INCLUDEPATH += ../../../../../Master/Include

DEPENDPATH += ../../../

SOURCES = TestDataManagerInclude.cpp

UseDepLibs1(Global DataLogging Threads NetCommands DeviceControl DataManager RemoteCareManager \
		   RemoteCareController HeartBeatManager EventHandler GPIOManager ExternalProcessController \
                   NetworkComponents StateMachines PasswordManager SWUpdateManager ExportController EncryptionDecryption)

#UseDepLibs()

LIBS += -ldl
