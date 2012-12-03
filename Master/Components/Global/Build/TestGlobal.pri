# include pri file from Master/Build

!include("../../../../../Colorado/ColoradoMain/Gui/Test/Build/Test.pri") {
    error("../../../Build/Platform.pri not found")
}
