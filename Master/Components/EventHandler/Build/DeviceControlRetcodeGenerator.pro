!include("DeviceControlRetcodeGenerator.pri") {
    error("DeviceControlRetcodeGenerator.pri not found")
}

TARGET = ReturnCodeGenerator

HEADERS =  ../Include/EventScenarioErrorXMLInfo.h \

SOURCES =  ../Source/EventScenarioErrorXMLInfo.cpp \
           ../Source/DeviceControlRetCodeGenerator.cpp \

QMAKE_POST_LINK = ../Source/genReturnCode.sh \
