# include pri file from Platform/Build

!include("../../../../../Himalaya/HimalayaMain/Gui/Test/Build/Test.pri") {
    error("../../../Build/Platform.pri not found")
}

QT += core \
      xmlpatterns \
      network
