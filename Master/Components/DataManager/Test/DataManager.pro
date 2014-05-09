!include("DataManager.pri") {
    error("DataManager.pri not found")
}

TEMPLATE = subdirs

SUBDIRS += TestDataManagerInclude.pro

CONFIG += ordered
