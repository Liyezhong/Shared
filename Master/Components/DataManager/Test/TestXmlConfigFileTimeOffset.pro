!include("DataManager.pri") {
    error("DataManager.pri not found")
}

TARGET = utTestXmlConfigFileTimeOffset

SOURCES += TestXmlConfigFileTimeOffset.cpp

UseLibs(DataManager Global DataLogging EventHandler)
