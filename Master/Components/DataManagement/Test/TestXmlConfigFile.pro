!include("DataManagement.pri") {
    error("DataManagement.pri not found")
}

TARGET = utTestXmlConfigFile

SOURCES += TestXmlConfigFile.cpp

UseLibs(DataManagement Global)
