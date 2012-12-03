# include pri file from Platform/Build

!include("../../../../../Colorado/ColoradoMain/Gui/Test/Build/Test.pri") {
    error("../../../Build/Platform.pri not found")
}

QT += core \
      xmlpatterns \
      network
