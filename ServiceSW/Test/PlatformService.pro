# build all components recursive

TEMPLATE = subdirs
SUBDIRS = ../../Master/Components/Global/Build/Global.pro \
          ../../../Platform/Gui/Components/Application/Build/Application.pro \
          ../../../Platform/Master/Components/EncryptionDecryption/Build/EncryptionDecryption.pro \
          ../../../Platform/ServiceSW/Components/TextViewer/Build/TextViewer.pro \
          ../../../Platform/ServiceSW/Components/Settings/Build/Settings.pro \
          ../../../Platform/ServiceSW/Components/ServiceKeyValidator/Build/ServiceKeyValidator.pro \
          ../../../Platform/ServiceSW/Components/ServiceImportExport/Build/ServiceImportExport.pro \
          ../../../Platform/ServiceSW/Components/ServiceImportExport/Test/TestServiceImportExportHandler.pro


CONFIG += ordered
