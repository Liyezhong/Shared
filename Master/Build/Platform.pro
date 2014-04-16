# build all components recursive

TEMPLATE = subdirs
SUBDIRS = ../Components/DataLogging/Build/DataLogging.pro \
          ../Components/DataManager/Build/DataManager.pro \
          ../Components/DeviceControl/Build/DeviceControl.pro \
          ../Components/DeviceCommandProcessor/Build/DeviceCommandProcessor.pro \
          ../Components/EventHandler/Build/EventHandler.pro \
          ../Components/ExternalProcessController/Build/ExternalProcessController.pro \
          ../Components/Global/Build/Global.pro \
          ../Components/ImportExport/Build/ImportExport.pro \
          ../Components/NetworkComponents/Build/NetworkComponents.pro \
          ../Components/StateMachines/Build/StateMachines.pro \
          ../Components/RemoteCareController/Build/RemoteCareController.pro \
          ../Components/Threads/Build/Threads.pro \
          ../Components/NetCommands/Build/NetCommands.pro \
          ../Components/PasswordManager/Build/PasswordManager.pro \
          ../Components/GPIOManager/Build/GPIOManager.pro \
          ../Components/HeartBeatManager/Build/HeartBeatManager.pro \

CONFIG += ordered
