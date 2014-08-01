CONFIG += qtestlib debug
TEMPLATE = app
TARGET = GenerateTestArchives
QT -= gui

MOC_DIR = moc
OBJECTS_DIR = obj

WRTOP = ../../..

INCLUDEPATH += ../../../..

HEADERS += $$WRTOP/WriteArchive/Include/WriteArchive.h
HEADERS += $$WRTOP/HMAC/Include/Hmac.h
HEADERS += $$WRTOP/AES/Include/AES.h
HEADERS += $$WRTOP/CryptoService/Include/CryptoService.h
HEADERS += $$WRTOP/CompressEncrypt/Include/CompressEncrypt.h
HEADERS += $$WRTOP/General/Include/General.h
HEADERS += GenerateTestArchives.h

SOURCES += $$WRTOP/WriteArchive/Source/WriteArchive.cpp
SOURCES += $$WRTOP/HMAC/Source/*.cpp
SOURCES += $$WRTOP/AES/Source/*.cpp
SOURCES += $$WRTOP/CryptoService/Source/*.cpp
SOURCES += $$WRTOP/CompressEncrypt/Source/*.cpp
SOURCES += $$WRTOP/General/Source/*.cpp
SOURCES += GenerateTestArchives.cpp
