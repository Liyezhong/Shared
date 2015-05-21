# build all components recursive

TEMPLATE = subdirs
SUBDIRS =   ../../Master/Components/Global/Build/Global.pro \
            ../../Master/Components/DataManager/Build/DataManager.pro \
            ../../Master/Components/EncryptionDecryption/Build/EncryptionDecryption.pro \
            ../Components/ExportData/Test/TestExportData.pro \
            ../Components/Main/Test/TestMain.pro \

CONFIG += ordered
