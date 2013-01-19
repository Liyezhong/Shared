# include pri file from Platform/Build

!include("../../../../../Colorado/ColoradoMain/Gui/GuiStub/Build/GuiStub.pri") {
    error("../../../Build/Platform.pri not found")
}

QT += core \
      xmlpatterns \
      network
