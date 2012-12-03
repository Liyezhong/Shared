!include("DataManager.pri") {
    error("DataManager.pri not found")
}

TARGET = utTestXmlConfigFilePasswords

SOURCES += TestXmlConfigFilePasswords.cpp

UseLibs(DataManager Global PasswordManager DataLogging EventHandler)
