!include("DeviceCommandProcessor.pri") {
    error("DeviceCommandProcessor.pri not found")
}

TEMPLATE = subdirs

#SUBDIRS =   TestBaseCommand.pro

CONFIG += ordered
