CONFIG += qtestlib
TEMPLATE = app
TARGET = utTestAes
QT -= gui

MOC_DIR = moc
OBJECTS_DIR = obj

INCLUDEPATH += ../../..

HEADERS += ../Include/Aes.h
HEADERS += TestAES.h

SOURCES += ../Source/Aes.cpp
SOURCES += RandData.cpp
SOURCES += VarkeyData.cpp
SOURCES += TestAES.cpp
