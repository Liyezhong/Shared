CONFIG += qtestlib
TEMPLATE = app
TARGET = utTestCryptoService
QT -= gui

MOC_DIR = moc
OBJECTS_DIR = obj

INCLUDEPATH += ../../..

HEADERS += ../Include/CryptoService.h
HEADERS += ../../HMAC/Include/Hmac.h
HEADERS += ../../AES/Include/AES.h
HEADERS += ../../General/Include/General.h
HEADERS += TestCryptoService.h

SOURCES += ../Source/CryptoService.cpp
SOURCES += ../../HMAC/Source/*.cpp
SOURCES += ../../AES/Source/*.cpp
SOURCES += ../../General/Source/*.cpp
SOURCES += TestCryptoService.cpp
