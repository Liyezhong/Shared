!include("DeviceCommandProcessor.pri"):error("DeviceCommandProcessor.pri not found")

QT += core gui
QT += network
QT += xmlpatterns

TARGET = DeviceCommandProcessor

HEADERS +=  ../Include/*.h \
            ../Include/Commands/*.h

SOURCES +=  ../Source/*.cpp \
            ../Source/Commands/*.cpp

INCLUDEPATH += ../../../../Common/Components
INCLUDEPATH += $$PWD/../../../../Components
INCLUDEPATH += ../../../../../Colorado/ColoradoMain/Master/Components
INCLUDEPATH += ../../../../../Colorado/Shared/Master/Components \

DEPENDPATH +=  ../../../../../Colorado/Shared/Master/Components \




#unix:!macx:!symbian: LIBS += -L$$OUT_PWD/../../../../../Colorado/ColoradoMain/Master/Components/Scheduler/Demo/SchedulerSimulation/ -lSchedulerSimulation

#INCLUDEPATH += $$PWD/../../../../../Colorado/ColoradoMain/Master/Components/Scheduler/Demo/SchedulerSimulation
#DEPENDPATH += $$PWD/../../../../../Colorado/ColoradoMain/Master/Components/Scheduler/Demo/SchedulerSimulation

#unix:!macx:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../../../../Colorado/ColoradoMain/Master/Components/Scheduler/Demo/SchedulerSimulation/libSchedulerSimulation.a







