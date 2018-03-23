!include("DeviceCommandProcessor.pri"):error("DeviceCommandProcessor.pri not found")

QT += core gui
QT += network
QT += xmlpatterns

TARGET = DeviceCommandProcessor

HEADERS +=  ../Include/*.h \
            ../Include/Commands/*.h

SOURCES +=  ../Source/*.cpp \
            ../Source/Commands/*.cpp

INCLUDEPATH += ../../../..../Shared/Common/Components
INCLUDEPATH += $$PWD/../../../../Components
INCLUDEPATH += ../../../../../Master/Components
INCLUDEPATH += ../../../../../Shared/Master/Components \

DEPENDPATH +=  ../../../../../Shared/Master/Components \




#unix:!macx:!symbian: LIBS += -L$$OUT_PWD/../../../../../Himalaya/HimalayaMain/Master/Components/Scheduler/Demo/SchedulerSimulation/ -lSchedulerSimulation

#INCLUDEPATH += $$PWD/../../../../../Himalaya/HimalayaMain/Master/Components/Scheduler/Demo/SchedulerSimulation
#DEPENDPATH += $$PWD/../../../../../Himalaya/HimalayaMain/Master/Components/Scheduler/Demo/SchedulerSimulation

#unix:!macx:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../../../../Himalaya/HimalayaMain/Master/Components/Scheduler/Demo/SchedulerSimulation/libSchedulerSimulation.a







