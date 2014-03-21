# include pri file from Master/Build

!include("../../../../../Himalaya/HimalayaMain/Gui/GuiStub/Build/GuiStub.pri") {
    error("../../../Build/Platform.pri not found")
}
