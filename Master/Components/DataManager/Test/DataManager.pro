!include("DataManager.pri") {
    error("DataManager.pri not found")
}

TEMPLATE = subdirs

SUBDIRS += TestDataManagerInclude.pro
SUBDIRS += TestCmdInterface.pro
SUBDIRS += TestDeviceConfiguration.pro
SUBDIRS += TestDeviceConfigurationSepia.pro
SUBDIRS += TestRCConfiguration.pro
SUBDIRS += TestExportConfiguration.pro
SUBDIRS += TestUserSettings.pro
SUBDIRS += TestDataModuleList.pro
SUBDIRS += TestSWVersionList.pro
SUBDIRS += TestXmlConfigFile.pro

CONFIG += ordered
