CONFIG += qtestlib
TEMPLATE = app
TARGET = utTestDecryptUncompress
QT -= gui

MOC_DIR = moc
OBJECTS_DIR = obj

INCLUDEPATH += ../../..
#INCLUDEPATH += ../Include
#INCLUDEPATH += ../../HMAC/Include
#INCLUDEPATH += ../../AES/Include
#INCLUDEPATH += ../../CryptoService/Include
#INCLUDEPATH += ../../General/Include

HEADERS += ../Include/DecryptUncompress.h
HEADERS += ../../HMAC/Include/Hmac.h
HEADERS += ../../AES/Include/AES.h
HEADERS += ../../CryptoService/Include/CryptoService.h
HEADERS += TestDecryptUncompress.h

SOURCES += ../Source/DecryptUncompress.cpp
SOURCES += ../../HMAC/Source/*.cpp
SOURCES += ../../AES/Source/*.cpp
SOURCES += ../../CryptoService/Source/*.cpp
SOURCES += ../../General/Source/*.cpp
SOURCES += TestDecryptUncompress.cpp
