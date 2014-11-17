CONFIG += qtestlib
TEMPLATE = app
TARGET = utTestDecryptUncompress
QT -= gui

# compute CONFIG_SUFFIX depending on debug / release
CONFIG(debug, debug|release) {
        CONFIG_SUFFIX = dbg
} else {
        CONFIG_SUFFIX = rel
        CONFIG(gcov) {
                QMAKE_CXXFLAGS_RELEASE += -fprofile-arcs -ftest-coverage
                QMAKE_LFLAGS_RELEASE += -fprofile-arcs
                QMAKE_CXXFLAGS_RELEASE -= -O2
                QMAKE_CXXFLAGS_RELEASE += -O0
                LIBS += -lgcov
        }
}

# set compile output directories
OBJECTS_DIR = obj_$$CONFIG_SUFFIX
MOC_DIR = moc_$$CONFIG_SUFFIX
DESTDIR = bin_$$CONFIG_SUFFIX

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
