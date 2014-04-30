!include("DeviceControl.pri") {
    error("DeviceControl.pri not found")
}

TEMPLATE = subdirs

SUBDIRS += TestIDeviceProcessing.pro

CONFIG += ordered
