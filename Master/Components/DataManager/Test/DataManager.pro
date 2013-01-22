!include("DataManager.pri") {
    error("DataManager.pri not found")
}

TEMPLATE = subdirs

SUBDIRS =   TestXmlConfigFile.pro \
            TestXmlConfigFilePasswords.pro \
            TestXmlConfigFileStrings.pro \
            TestXmlConfigFileTimeOffset.pro \
            ../Containers/Racks/Test/TestRackList.pro \
            ../Containers/UserSettings/Test/TestUserSettings.pro \
            ../Containers/ExportConfiguration/Test/TestExportConfiguration.pro \
            ../Containers/SWVersions/Test/TestSWVersionList.pro \
            ../Containers/DeviceConfiguration/Test/TestDeviceConfiguration.pro \
            ../Containers/InstrumentHistory/Test/TestDataModuleList.pro
#TestPasswordManager.pro \
