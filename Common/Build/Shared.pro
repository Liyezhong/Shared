# build all components recursive

TEMPLATE = subdirs

SUBDIRS  = ../Components/HimalayaDataContainer/Build/HimalayaDataContainer.pro \
		   ../Components/RMSCommands/Build/RMSCommands.pro

CONFIG += ordered
