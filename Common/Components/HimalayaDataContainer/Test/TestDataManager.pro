!include("TestDataManager.pri") {
    error("TestDataManager.pri not found")
}

TEMPLATE = subdirs
#SUBDIRS  = ../Containers/ReagentStations/Test/TestDataStationList.pro
#SUBDIRS += ../Containers/Reagents/Test/ TestDataReagentList.pro
#SUBDIRS += ../Containers/ReagentGroups/Test/TestDataReagentGroupList.pro
#SUBDIRS += ../Containers/Programs/Test/TestDataProgramList.pro
SUBDIRS += ../Containers/ProgramSettings/Test/TestProgramSettings.pro
SUBDIRS += ../Containers/UserSettings/Test/TestUserSettings/TestUserSettings.pro
		  
CONFIG += ordered


INCLUDEPATH += ../../../../HimalayaMain/Master/Components/

SUBDIRS += \
    ../Containers/UserSettings/Test/TestUserSettings
