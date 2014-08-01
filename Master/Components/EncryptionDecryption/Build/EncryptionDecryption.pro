!include("EncryptionDecryption.pri") {
    error("EncryptionDecryption.pri not found")
}

TARGET = EncryptionDecryption

INCLUDEPATH += ../../..

HEADERS += ../ReadArchive/Include/ReadArchive.h
HEADERS += ../WriteArchive/Include/WriteArchive.h
HEADERS += ../HMAC/Include/Hmac.h
HEADERS += ../AES/Include/AES.h
HEADERS += ../CryptoService/Include/CryptoService.h
HEADERS += ../AbstractFile/Include/AbstractFile.h
HEADERS += ../CompressEncrypt/Include/CompressEncrypt.h
HEADERS += ../DecryptUncompress/Include/DecryptUncompress.h
HEADERS += ../General/Include/General.h


SOURCES += ../ReadArchive/Source/*.cpp
SOURCES += ../WriteArchive/Source/*.cpp
SOURCES += ../HMAC/Source/*.cpp
SOURCES += ../AES/Source/*.cpp
SOURCES += ../CryptoService/Source/*.cpp
SOURCES += ../AbstractFile/Source/*.cpp
SOURCES += ../CompressEncrypt/Source/*.cpp
SOURCES += ../DecryptUncompress/Source/*.cpp
SOURCES += ../General/Source/*.cpp
