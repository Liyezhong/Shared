!include("../../../Test/TestDataManager.pri") {
    error("TestDataManager.pri not found")
}


TARGET = utTestDataProgramList

SOURCES += TestDataProgramList.cpp

INCLUDEPATH += ../../../../ \
               ../../../../../../../../Platform/Master/Components \
               ../../../../../../../HimalayaMain/Master/Include/


INCLUDEPATH +=  ../../../../
INCLUDEPATH +=  ../../../../../../../HimalayaMain/Master/Components/

RESOURCES += ../Resources/*.qrc

UseLib(../../../../../../../HimalayaMain/Master/Components/HimalayaDataManager)
UseLib(../../../../../../../../Platform/Master/Components/DataManager ../../../../../../../../Platform/Master/Components/Global)
#CONFIG  += qtestlib
