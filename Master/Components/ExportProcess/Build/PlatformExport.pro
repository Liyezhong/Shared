# build all components recursive

TEMPLATE = subdirs
SUBDIRS = ../Components/ExportData/Build/ExportData.pro \
          ../Components/Main/Build/Main.pro

CONFIG += ordered
