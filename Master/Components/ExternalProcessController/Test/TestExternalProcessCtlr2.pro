!include("ExternalProcessTest.pri") {
    error("ExternalProcessTest.pri not found")
}

QT += xml \
      xmlpatterns \
      network

TARGET = utTestExternalProcessCtlr2

HEADERS += TestExternalProcessCtlr2.h \
           DerivedController.h

SOURCES += TestExternalProcessCtlr2.cpp \
           DerivedController.cpp

UseLibs(Global EventHandler DataLogging StateMachines NetworkComponents ExternalProcessController Threads)

