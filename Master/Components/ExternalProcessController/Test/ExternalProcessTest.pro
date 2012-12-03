!include("ExternalProcessTest.pri") {
    error("ExternalProcessTest.pri not found")
}

TEMPLATE = subdirs

SUBDIRS += TestExternalProcess.pro \
           TestExternalProcessCtlr.pro \
           TestExternalProcessCtlr2.pro \
           TestExternalProcessDvc.pro

# And now for something completely different
SUBDIRS += CpuBandwidthEater.pro

CONFIG += ordered
