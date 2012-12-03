!include("DeviceControl.pri") {
    error("DeviceControl.pri not found")
}

TARGET = DeviceControl

INCLUDEPATH += ../../../../Common/Components

HEADERS +=  ../Include/CanCommunication/*.h \
            ../Include/CanCommunication/CanTcpCommunication/*.h \
            ../Include/SlaveModules/*.h \
            ../Include/Configuration/*.h \
            ../Include/DeviceProcessing/*.h \
            ../Include/Devices/*.h \
            ../Include/Global/*.h \
            ../Include/Interface/*.h \
            ../../../../Common/Components/FunctionModules/*.h \

SOURCES +=  ../Source/CanCommunication/*.cpp \
            ../Source/CanCommunication/CanTcpCommunication/*.cpp \
            ../Source/SlaveModules/*.cpp \
            ../Source/Configuration/*.cpp \
            ../Source/DeviceProcessing/*.cpp \
            ../Source/Devices/*.cpp \
            ../Source/Interface/*.cpp
