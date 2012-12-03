CONFIG += qtestlib
TEMPLATE = app
TARGET = utTestAes
QT -= gui

MOC_DIR = moc
OBJECTS_DIR = obj

INCLUDEPATH += ../../..

HEADERS += ../Include/Aes.h
HEADERS += TestAes.h

SOURCES += ../Source/Aes.cpp
SOURCES += RandData.cpp
SOURCES += VarkeyData.cpp
SOURCES += TestAes.cpp
