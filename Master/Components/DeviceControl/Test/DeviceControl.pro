!include("DeviceControl.pri") {
    error("DeviceControl.pri not found")
}

TEMPLATE = subdirs

SUBDIRS += TestDeviceControl.pro

CONFIG += ordered
