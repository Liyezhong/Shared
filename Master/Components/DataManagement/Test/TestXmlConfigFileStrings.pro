!include("DataManagement.pri") {
    error("DataManagement.pri not found")
}

TARGET = utTestXmlConfigFileStrings

SOURCES += TestXmlConfigFileStrings.cpp

UseLibs(DataManagement Global)
