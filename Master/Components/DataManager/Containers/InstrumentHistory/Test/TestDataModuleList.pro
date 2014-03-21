#-------------------------------------------------
#
# Project created by QtCreator 2013-01-09T17:16:51
#
#-------------------------------------------------

!include("../../../Test/DataManager.pri"):error("DataManager.pri not found")

TARGET = utTestDataModuleList

INCLUDEPATH += ../../../../ \
 ../../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../../ \

UseLib(../../../../DataManager ../../../../Global)

RESOURCES += \
    ../Resources/Resources.qrc

SOURCES += \
    TestDataModuleList.cpp

