# build all components recursive

TEMPLATE = subdirs
SUBDIRS = ../Components/Application/Build/Application.pro
SUBDIRS = ../Components/KeyBoard/Build/KeyBoard.pro
SUBDIRS = ../Components/MainMenu/Build/MainMenu.pro


CONFIG += ordered
