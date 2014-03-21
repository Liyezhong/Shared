!include("DataManager.pri") {
    error("DataManager.pri not found")
}

TARGET = utTestXmlConfigFileStrings

SOURCES += TestXmlConfigFileStrings.cpp

UseLibs(DataManager Global DataLogging EventHandler)
