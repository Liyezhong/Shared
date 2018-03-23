# build all components recursive

TEMPLATE = subdirs

PLATFORM_COMPONENTS_PATH = ../../Platform/Master/Components

######## normal build process ########
#
# Make sure your *.pro files are in the last line to ensure correct linking!
#

SUBDIRS  =  $$PLATFORM_COMPONENTS_PATH/NetworkComponents/Build/NetworkComponents.pro \
            $$PLATFORM_COMPONENTS_PATH/NetCommands/Build/NetCommands.pro \
            $$PLATFORM_COMPONENTS_PATH/Global/Build/Global.pro \
            $$PLATFORM_COMPONENTS_PATH/DataManager/Build/DataManager.pro \
            ../../Shared/Common/Components/HimalayaDataContainer/Build/HimalayaDataContainer.pro \
            REServerConnectorTest.pro \
            RCConfigurationWrapperTest.pro \
            MasterConnectorTest.pro \
            AgentControllerTest.pro \

CONFIG += ordered

