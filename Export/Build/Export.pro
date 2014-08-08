# build all components recursive

TEMPLATE = subdirs
SUBDIRS = ../../Master/Components/Global/Build/Global.pro \
          ../../Master/Components/DataManager/Build/DataManager.pro \
          ../../Master/Components/EncryptionDecryption/Build/EncryptionDecryption.pro \
          ../Components/ExportData/Build/ExportData.pro \
          ../Components/Main/Build/Main.pro

CONFIG += ordered
