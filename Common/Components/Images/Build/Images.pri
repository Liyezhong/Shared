# include pri file from Gui/Build

!include("../../../Build/HimalayaSharedGui.pri") {
    error("../../../Build/HimalayaSharedGui.pri not found")
}
