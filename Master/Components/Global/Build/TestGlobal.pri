# include pri file from Master/Build

!include("../../../../../Colorado/ColoradoMain/Gui/GuiStub/Build/GuiStub.pri") {
    error("../../../Build/Platform.pri not found")
}
