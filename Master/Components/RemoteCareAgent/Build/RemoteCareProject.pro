# build all components recursive

TEMPLATE = subdirs

PLATFORM_COMPONENTS_PATH = ../../../Components

######## normal build process ############################################################
#
# Make sure your RemoteCareAgent.pro is in the last line to ensure correct linking!
#
SUBDIRS  =  $$PLATFORM_COMPONENTS_PATH/NetworkComponents/Build/NetworkComponents.pro \
            $$PLATFORM_COMPONENTS_PATH/Global/Build/Global.pro \
            ./RemoteCareAgent.pro

CONFIG += ordered
