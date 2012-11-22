!include("DataManagement.pri") {
    error("DataManagement.pri not found")
}

TARGET = utTestXmlConfigFileTimeOffset

SOURCES += TestXmlConfigFileTimeOffset.cpp

UseLibs(DataManagement Global)
