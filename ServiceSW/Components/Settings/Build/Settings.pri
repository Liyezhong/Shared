# include pri file from PlatformServiceSW/Build

!include("../../../Build/PlatformServiceSW.pri") {
    error("../../../Build/PlatformServiceSW.pri not found")
}
