!include("DataManager.pri") {
    error("DataManager.pri not found")
}

TARGET = utTestXmlConfigFile

SOURCES += TestXmlConfigFile.cpp

UseLibs(DataManager Global DataLogging EventHandler)
