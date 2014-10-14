#-------------------------------------------------
#
# Project created by QtCreator 2013-01-09T17:16:51
#
#-------------------------------------------------

!include("DataManager.pri"):error("DataManager.pri not found")

QT += xml
QT += xmlpatterns
QT += network

TARGET = utTestCmdInterface

INCLUDEPATH += ../../../ \
 ../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../ \


SOURCES = TestCmdInterface.cpp

UseDepLibs(Global DataLogging Threads NetCommands DeviceControl DataManager RemoteCareManager \
		   RemoteCareController HeartBeatManager EventHandler GPIOManager ExternalProcessController \
                   NetworkComponents StateMachines PasswordManager SWUpdateManager ExportController EncryptionDecryption)
LIBS += -ldl
