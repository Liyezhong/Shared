!include("../../../Test/TestDataManager.pri") {
    error("TestDataManager.pri not found")
}

TARGET = utTestDataStationList

SOURCES += TestDataStationList.cpp

INCLUDEPATH += ../../../../ \
 ../../../../../../../../Platform/Master/Components/ \
 ../../../../../../../HimalayaMain/Master/Components/
DEPENDPATH +=  ../../../../

RESOURCES += ../Resources/*.qrc
UseLib(../../../../HimalayaDataContainer)
UseLib(../../../../../../../../Platform/Master/Components/DataManager ../../../../../../../../Platform/Master/Components/Global)
#CONFIG  += qtestlib
