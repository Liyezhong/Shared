# build all components recursive

TEMPLATE = subdirs
SUBDIRS =   ../Components/ExportData/Test/TestExportData.pro \
            ../Components/Main/Test/TestMain.pro \

CONFIG += ordered
