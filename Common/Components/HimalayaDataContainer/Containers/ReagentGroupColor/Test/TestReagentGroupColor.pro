!include("../../../Test/TestDataManager.pri") {
    error("TestDataManagement.pri not found")
}

TARGET = utTestReagentGroupColor

SOURCES += TestReagentGroupColor.cpp

INCLUDEPATH += ../../../../ \
                ../../../../../../../../Platform/Master/Components \
                ../../../../../../../HimalayaMain/Master/Include/


DEPENDPATH +=  ../../../../

UseLib(../../../../HimalayaDataContainer)
UseLib(../../../../../../../../Platform/Master/Components/DataManager ../../../../../../../../Platform/Master/Components/Global)

RESOURCES += \
    ../Resource/ReagentGroupColorXML.qrc
