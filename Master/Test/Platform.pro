# build all components recursive

TEMPLATE = subdirs
SUBDIRS =   ../Components/Global/Test/Global.pro \
            ../Components/DataLogging/Test/DataLogging.pro \
            ../Components/DataManagement/Test/DataManagement.pro \
            ../Components/EventHandler/Test/EventHandler.pro \
            ../Components/ExternalUtilities/Test/ExternalUtilities.pro \
            ../Components/DeviceCommandProcessor/Test/DeviceCommandProcessor.pro \
            ../Components/StateMachines/Test/StateMachines.pro \
            ../Components/ImportExport/Test/ImportExport.pro 

            #../Components/ExternalProcessController/Test/ExternalProcessTest.pro \
            #../Components/DeviceControl/Test/DeviceControl.pro \
            #../Components/NetworkComponents/Test/NetworkComponentsTest.pro

CONFIG += ordered
