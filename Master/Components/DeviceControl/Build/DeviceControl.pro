!include("DeviceControl.pri") {
    error("DeviceControl.pri not found")
}

TARGET = DeviceControl

DEFINES += PRE_ALFA_TEST
DEFINES += HAL_CV_TEST
INCLUDEPATH += ../../../../../Shared/Common/Components\


HEADERS +=  ../Include/CanCommunication/*.h \
            ../Include/CanCommunication/CanTcpCommunication/*.h \
            ../Include/SlaveModules/*.h \
            ../Include/Configuration/*.h \
            ../Include/DeviceProcessing/*.h \
            ../Include/Devices/*.h \
            ../Include/Global/*.h \
            ../Include/Interface/*.h \
            ../Include/Simulation/*.h \
            ../../../../../Shared/Common/Components/FunctionModules/*.h \
            ../hwconfig/*.hpp \
#    DeviceModuleModel.h

SOURCES +=  ../Source/CanCommunication/*.cpp \
            ../Source/CanCommunication/CanTcpCommunication/*.cpp \
            ../Source/SlaveModules/*.cpp \
            ../Source/Configuration/*.cpp \
            ../Source/DeviceProcessing/*.cpp \
            ../Source/Devices/*.cpp \
            ../Source/Interface/*.cpp \
            ../Source/Simulation/*.cpp \
            ../hwconfig/*.cpp \

#    DeviceModuleModel.cpp


QMAKE_CXXFLAGS += -Wall -Wno-strict-aliasing

