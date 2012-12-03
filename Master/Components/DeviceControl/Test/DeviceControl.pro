!include("DeviceControl.pri") {
    error("DeviceControl.pri not found")
}

TEMPLATE = subdirs

SUBDIRS +=  TestOven/TestOven.pro \
            TestDrawer/TestDrawer.pro \
            TestRackTransfer/TestRackTransfer.pro \
            TestAgitation/TestAgitation.pro \
            TestHood/TestHood.pro

CONFIG += ordered
