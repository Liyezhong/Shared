# include pri file from Master/Build

!include("../../../Build/PlatformGui.pri") {
    error("../../../Build/PlatformGui.pri not found")
}
