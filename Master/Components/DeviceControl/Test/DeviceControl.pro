!include("DeviceControl.pri") {
    error("DeviceControl.pri not found")
}

TEMPLATE = subdirs

SUBDIRS += TestIDeviceProcessing.pro \
           TestDeviceControlSim.pro

CONFIG += ordered

