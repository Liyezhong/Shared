!include("../../../Test/DataManager.pri"):error("DataManager.pri not found")

TARGET = utTestRackList
SOURCES += TestRackList.cpp

INCLUDEPATH += ../../../../ \
 ../../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../../ \

UseLib(../../../../DataManager ../../../../Global)
