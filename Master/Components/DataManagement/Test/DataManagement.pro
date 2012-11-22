!include("DataManagement.pri") {
    error("DataManagement.pri not found")
}

TEMPLATE = subdirs

SUBDIRS =   TestPasswordManager.pro \
            TestXmlConfigFile.pro \
            TestXmlConfigFilePasswords.pro \
            TestXmlConfigFileStrings.pro \
            TestXmlConfigFileTimeOffset.pro \

