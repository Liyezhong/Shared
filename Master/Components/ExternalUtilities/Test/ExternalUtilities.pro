!include("ExternalUtilities.pri") {
    error("ExternalUtilities.pri not found")
}

TEMPLATE = subdirs

SUBDIRS += TestExternalUtilities.pro
SUBDIRS += TestGregorianDate.pro
SUBDIRS += TestTime.pro
SUBDIRS += TestTimeSpan.pro

CONFIG += ordered
