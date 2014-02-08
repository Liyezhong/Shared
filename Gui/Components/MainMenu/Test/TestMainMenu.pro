!include("TestMainMenu.pri"):error("TestMainMenu.pri not found")

TARGET = utTestMainMenu
SOURCES += TestMainMenu.cpp
TEMPLATE = app

CONFIG += static \
          warn_on \
          qt \
          rtti

QT += gui \
      core \
      xml \
      xmlpatterns

INCLUDEPATH += ../../ \
            ../../../../../Platform/Master/Components \

CONFIG += qtestlib
CONFIG += qt
DEPENDPATH += ../../../../
UseLibs(MainMenu Application)
UseLib(../../../../Master/Components/DataManager)
UseLib(../../../../Master/Components/Global)
LIBS += ../../../../../Platform/Gui/Components/kineticscroller/lib/libQtScroller.a
