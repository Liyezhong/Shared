!include("WriteArchive.pri") {
    error("WriteArchive.pri not found")
}

# LOACL BUILD: CALL MAKE IN ../../Build before!!

CONFIG += qtestlib
TEMPLATE = app
TARGET = utTestWriteArchive
QT -= gui

MOC_DIR = moc
OBJECTS_DIR = obj

INCLUDEPATH += ../../..

HEADERS += ../Include/WriteArchive.h
HEADERS += ../../HMAC/Include/Hmac.h
HEADERS += ../../AES/Include/AES.h
HEADERS += ../../CryptoService/Include/CryptoService.h
HEADERS += ../../CompressEncrypt/Include/CompressEncrypt.h
HEADERS += ../../General/Include/General.h
HEADERS += TestWriteArchive.h

SOURCES += TestWriteArchive.cpp

UseLibs(EncryptionDecryption)
#LIBS += ../../Build/lib_dbg/libEncryptionDecryption.a
