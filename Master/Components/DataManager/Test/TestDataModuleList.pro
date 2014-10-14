!include("DataManager.pri"):error("DataManager.pri not found")

TARGET = utTestDataModuleList

INCLUDEPATH += ../../../ \
 ../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../
RESOURCES += ../Containers/InstrumentHistory/Resources/*.qrc


SOURCES = TestDataModuleList.cpp

UseDepLibs(Global DataLogging Threads NetCommands DeviceControl DataManager RemoteCareManager \
		   RemoteCareController HeartBeatManager EventHandler GPIOManager ExternalProcessController \
                   NetworkComponents StateMachines PasswordManager SWUpdateManager ExportController EncryptionDecryption)

LIBS += -ldl
