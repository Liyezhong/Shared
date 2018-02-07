#-------------------------------------------------
#
# Project created by QtCreator 2014-11-17T15:42:55
#
#-------------------------------------------------
HIMALAYA                =  ../../../../../../../../../Himalaya
HIMALAYA_MAIN     = $${HIMALAYA}/HimalayaMain
SHARED                    =  $${HIMALAYA}/Shared
PLATFORM               =  ../../../../../../../../../Platform

!include("$${SHARED}/Master/Components/HimalayaDataContainer/Test/TestDataManager.pri"):error("DataManager.pri not found")

QT       += testlib

QT       -= gui

TARGET = utTestUserSettings
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += $${PLATFORM}/Master/Components/
INCLUDEPATH += $${SHARED}/Master/Components/

UseLib($${PLATFORM}/Master/Components/DataManager $${PLATFORM}/Master/Components/Global $${HIMALAYA}/Shared/Master/Components/HimalayaDataContainer)

SOURCES += TestUserSettings.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
