# build all components recursive

TEMPLATE = subdirs

PLATFORM_COMPONENTS_PATH = ../../Platform/Master/Components

######## normal build process ########
#
# Make sure your Main.pro is in the last line to ensure correct linking!
#

SUBDIRS  =  ../../Himalaya/Shared/Master/Components/HimalayaDataContainer/Build/HimalayaDataContainer.pro \
            $$PLATFORM_COMPONENTS_PATH/NetworkComponents/Build/NetworkComponents.pro \
            $$PLATFORM_COMPONENTS_PATH/NetCommands/Build/NetCommands.pro \
            $$PLATFORM_COMPONENTS_PATH/Global/Build/Global.pro \
            $$PLATFORM_COMPONENTS_PATH/DataManager/Build/DataManager.pro \
            Main.pro

CONFIG += ordered

