# build all components recursive

TEMPLATE = subdirs
SUBDIRS = ../../../../Master/Components/Global/Build/Global.pro \
          ../../../../Gui/Components/Application/Build/Application.pro \
        ../Build/KeyBoard.pro \
       ../../MainMenu/Build/MainMenu.pro \
       ./MainTest/Build/MainTest.pro

CONFIG += ordered
