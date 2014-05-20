!include("../../../../../Platform/ServiceSW/Test/PlatformService.pri"):error("PlatformService.pri not found")

TARGET = utTestServiceImportExportHandler
SOURCES += TestServiceImportExportHandler.cpp

INCLUDEPATH += ../../../../ \
 ../../../../../Platform/Master/Components/

DEPENDPATH += ../../../../
	

UseLibs(ServiceImportExport)
UsePlatformServiceLibs(TextViewer Settings ServiceKeyValidator)
UsePlatformLibs(Threads EventHandler Global DataManager NetCommands StateMachines NetworkComponents DeviceControl DeviceCommandProcessor GPIOManager EncryptionDecryption DataLogging ExternalProcessController ExportController PasswordManager)
UsePlatformGUILibs(MainMenu KeyBoard Application)
# UseSharedLibs(ColoradoDataContainer)
UsePlatformScroller(QtScroller)
