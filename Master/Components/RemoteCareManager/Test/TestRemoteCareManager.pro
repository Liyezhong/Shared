!include("TestRemoteCareManager.pri"){error("TestRemoteCareManager.pri not found")}

TARGET = utTestRemoteCareManager
SOURCES += TestRemoteCareManager.cpp

QT += xml
QT += xmlpatterns
QT += network

INCLUDEPATH += ../../
DEPENDPATH += ../../

UseLibs(DataLogging DataManager SWUpdateManager EventHandler ExternalProcessController Global NetworkComponents StateMachines RemoteCareController RemoteCareManager NetCommands Threads GPIOManager HeartBeatManager)

LIBS += ../../../../../Himalaya/Shared/Master/Components/HimalayaDataContainer/Build/lib_$${CONFIG_SUFFIX}/libHimalayaDataContainer.a

RESOURCES += \
    ../Resources/Resources.qrc

