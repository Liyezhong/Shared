# build all components recursive

TEMPLATE = subdirs
SUBDIRS = ../Components/Application/Build/Application.pro \
          ../Components/KeyBoard/Build/KeyBoard.pro \
          ../Components/MainMenu/Build/MainMenu.pro


CONFIG += ordered
