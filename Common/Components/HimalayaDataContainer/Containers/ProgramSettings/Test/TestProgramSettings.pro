!include("../../../Test/TestDataManager.pri"):error("DataManager.pri not found")

HIMALAYA                =  ../../../../../../../../Himalaya
HIMALAYA_MAIN     = $${HIMALAYA}/HimalayaMain
SHARED                    =  $${HIMALAYA}/Shared
PLATFORM               =  ../../../../../../../../Platform


INCLUDEPATH += ../../../../ \
 ../../../../../../../../Platform/Master/Components/
INCLUDEPATH += $${HIMALAYA}/HimalayaMain/Master/Include/

DEPENDPATH += ../../../../ \

QT       += testlib
QT       -= gui

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


TARGET = utTestProgramSettings
SOURCES += TestProgramSettings.cpp

#UseLib(../../../../DataManager ../../../../Global)

UseLib($${PLATFORM}/Master/Components/DataManager $${PLATFORM}/Master/Components/Global $${HIMALAYA}/Shared/Master/Components/HimalayaDataContainer)

RESOURCES += \
    ../Resources/ProgramSettings.qrc

OTHER_FILES += \
    ../Resources/ProgramSettings.xml
