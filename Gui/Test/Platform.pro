# build all components recursive

TEMPLATE = subdirs
SUBDIRS = ../../Master/Components/Global/Build/Global.pro \
       ../Components/kineticscroller/qtscroller.pro \
       ../Components/KeyBoard/Build/KeyBoard.pro \
       ../Components/MainMenu/Build/MainMenu.pro \
       ../Components/KeyBoard/Test/TestKeyBoard.pro \
       ../Components/MainMenu/Test/TestMainMenu.pro

CONFIG += ordered
