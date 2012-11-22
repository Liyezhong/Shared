# include pri file from PlatformGui/Build

!include("../../../Build/PlatformGui.pri") {
    error("../../../Build/PlatformGui.pri not found")
}
