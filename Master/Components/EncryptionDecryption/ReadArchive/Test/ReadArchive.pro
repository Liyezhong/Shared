!include("ReadArchive.pri") {
    error("ReadArchive.pri not found")
}

# LOACL BUILD: CALL MAKE IN ../../Build before!!

CONFIG += qtestlib
TEMPLATE = app
TARGET = utTestReadArchive
QT -= gui

MOC_DIR = moc
OBJECTS_DIR = obj

INCLUDEPATH += ../../..

HEADERS += ../Include/ReadArchive.h
HEADERS += ../../HMAC/Include/Hmac.h
HEADERS += ../../AES/Include/AES.h
HEADERS += ../../CryptoService/Include/CryptoService.h
HEADERS += ../../AbstractFile/Include/AbstractFile.h
HEADERS += ../../DecryptUncompress/Include/DecryptUncompress.h
HEADERS += ../../General/Include/General.h
HEADERS += TestReadArchive.h

SOURCES += TestReadArchive.cpp

UseLibs(EncryptionDecryption)
#LIBS += ../../Build/lib_dbg/libEncryptionDecryption.a
