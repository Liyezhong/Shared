CONFIG += qtestlib
TEMPLATE = app
TARGET = utTestHmac
QT -= gui

MOC_DIR = moc
OBJECTS_DIR = obj

INCLUDEPATH += ../../..
#INCLUDEPATH += ../Include

HEADERS += ../Include/Hmac.h
HEADERS += TestHmac.h

SOURCES += ../Source/Hmac.cpp
SOURCES += TestHmac.cpp
