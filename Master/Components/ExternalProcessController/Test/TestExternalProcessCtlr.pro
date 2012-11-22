!include("ExternalProcessTest.pri") {
    error("ExternalProcessTest.pri not found")
}

QT += xml \
      xmlpatterns \
      network

TARGET = utTestExternalProcessCtlr

HEADERS += TestExternalProcessCtlr.h \
           DerivedController.h

SOURCES += TestExternalProcessCtlr.cpp \
           DerivedController.cpp

UseLibs(Global EventHandler DataLogging StateMachines NetworkComponents ExternalProcessController Threads)

