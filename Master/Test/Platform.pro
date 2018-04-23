# build all components recursive

TEMPLATE = subdirs
SUBDIRS =   ../../../Master/Components/Scheduler/Test/Scheduler.pro \
            ../../../Master/Components/Scheduler/Build/Scheduler.pro \
            ../Components/Global/Test/Global.pro \
            #../Components/DataLogging/Test/DataLogging.pro \
            #../Components/DataManager/Test/DataManager.pro \
            ../Components/EventHandler/Test/EventHandler.pro \
            #../Components/ExternalUtilities/Test/ExternalUtilities.pro \
            ../Components/DeviceCommandProcessor/Test/DeviceCommandProcessor.pro \
            ../Components/StateMachines/Test/StateMachines.pro \
            #../Components/ImportExport/Test/ImportExport.pro \
            ../Components/PasswordManager/Test/PasswordManager.pro \
            #../Components/ExternalProcessController/Test/ExternalProcessTest.pro \
            ../Components/DeviceControl/Test/DeviceControl.pro \
            ../Components/DeviceControl/Build/DeviceControl.pro \
            #../Components/NetworkComponents/Test/NetworkComponentsTest.pro
../../../Master/Components/Scheduler/Test/TestSchedulerCommandProcessor.pro\


CONFIG += ordered
