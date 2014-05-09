!include("DataManager.pri"):error("DataManager.pri not found")

QT += xml
QT += xmlpatterns
QT += network


TARGET = utTestDataManagerInclude.pro

#INCLUDEPATH += ../..
#INCLUDEPATH += ../Include
#INCLUDEPATH += ../../../../Common/Components/

SOURCES += TestDataManagerInclude.cpp

#HEADERS += ../Include/Interface/*.h
#HEADERS += ../Include/Global/*.h

UseLibs(Global DataManager)
