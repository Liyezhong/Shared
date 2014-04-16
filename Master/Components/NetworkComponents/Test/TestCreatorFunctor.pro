#-------------------------------------------------
#
# Project created by QtCreator 2013-01-09T17:16:51
#
#-------------------------------------------------

!include("NetworkComponentsTest.pri") {
    error("NetworkComponentsTest.pri not found")
}

QT += xml \
      xmlpatterns \
      network

TARGET = utTestCreatorFunctor

#HEADERS += TestCreatorFunctor.h

SOURCES += TestCreatorFunctor.cpp

UseLibs(Global DataLogging NetworkComponents)


