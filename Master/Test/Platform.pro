# build all components recursive

TEMPLATE = subdirs
SUBDIRS =   ../Components/Global/Test/Global.pro \
            #../Components/DataLogging/Test/DataLogging.pro \
            #../Components/DataManager/Test/DataManager.pro \
            ../Components/EventHandler/Test/EventHandler.pro \
            ../Components/ExternalUtilities/Test/ExternalUtilities.pro \
            ../Components/DeviceCommandProcessor/Test/DeviceCommandProcessor.pro \
            ../Components/StateMachines/Test/StateMachines.pro \
            ../Components/ImportExport/Test/ImportExport.pro \
            ../Components/PasswordManager/Test/PasswordManager.pro \
            #../Components/ExternalProcessController/Test/ExternalProcessTest.pro \
            #../Components/DeviceControl/Test/DeviceControl.pro \
            #../Components/NetworkComponents/Test/NetworkComponentsTest.pro

CONFIG += ordered
