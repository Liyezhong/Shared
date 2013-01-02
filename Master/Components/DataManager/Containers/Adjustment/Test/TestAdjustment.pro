!include("../../../Test/DataManager.pri"):error("DataManager.pri not found")

TARGET = utTestAdjustment
SOURCES += TestAdjustment.cpp

INCLUDEPATH += ../../../../ \
 ../../../../../../../../Platform/Master/Components/

DEPENDPATH += ../../../../ \

UseLib(../../../../DataManager ../../../../Global)
