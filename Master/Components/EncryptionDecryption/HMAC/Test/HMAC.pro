CONFIG += qtestlib
TEMPLATE = app
TARGET = utTestHmac
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

INCLUDEPATH += ../../..
#INCLUDEPATH += ../Include

HEADERS += ../Include/Hmac.h
HEADERS += ../../General/Include/*.h
HEADERS += TestHmac.h

SOURCES += ../Source/Hmac.cpp
SOURCES += ../../General/Source/*.cpp
SOURCES += TestHmac.cpp

