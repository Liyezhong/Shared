######################################################################
# Automatically generated by qmake (2.01a) Mi. Jul 21 14:13:36 2010
######################################################################

TEMPLATE = app
TARGET = gentest
DEPENDPATH += .
INCLUDEPATH += ../../..
INCLUDEPATH += ../../../Include
DEPENDPATH = ../../..
PRE_TARGETDEPS += ../libXMLParserTestlib.a
QT += xml
QT += testlib
QT -= gui

# Input
HEADERS += ../../Include/*.h
LIBS += ../libXMLParserTestlib.a
SOURCES += ../CtrlLsts.cpp
SOURCES += ../FormatString.cpp
SOURCES += *.cpp
QMAKE_CXXFLAGS += -DGENERALXMLDESCTEST_CPP
