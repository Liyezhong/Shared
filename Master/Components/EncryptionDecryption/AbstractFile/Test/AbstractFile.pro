CONFIG += qtestlib
TEMPLATE = app
TARGET = utTestAbstractFile
QT -= gui

MOC_DIR = moc
OBJECTS_DIR = obj

INCLUDEPATH += ../../..
#INCLUDEPATH += ../Include
#INCLUDEPATH += ../../General/Include

HEADERS += ../Include/AbstractFile.h
HEADERS += ../../General/Include/General.h
HEADERS += TestAbstractFile.h

SOURCES += ../../General/Source/*.cpp
SOURCES += ../Source/*.cpp
SOURCES += TestAbstractFile.cpp
