CONFIG += qtestlib
TEMPLATE = app
TARGET = utTestCompressEncrypt
QT -= gui

MOC_DIR = moc
OBJECTS_DIR = obj

INCLUDEPATH += ../../..

HEADERS += ../Include/CompressEncrypt.h
HEADERS += ../../HMAC/Include/Hmac.h
HEADERS += ../../AES/Include/AES.h
HEADERS += ../../CryptoService/Include/CryptoService.h
HEADERS += TestCompressEncrypt.h

SOURCES += ../Source/CompressEncrypt.cpp
SOURCES += ../../HMAC/Source/*.cpp
SOURCES += ../../AES/Source/*.cpp
SOURCES += ../../CryptoService/Source/*.cpp
SOURCES += ../../General/Source/*.cpp
SOURCES += TestCompressEncrypt.cpp
