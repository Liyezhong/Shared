!include("DataManagement.pri") {
    error("DataManagement.pri not found")
}

TARGET = utTestXmlConfigFilePasswords

SOURCES += TestXmlConfigFilePasswords.cpp

UseLibs(DataManagement Global)
