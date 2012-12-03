# include pri file from Platform/Export/Build

!include("../../../Build/PlatformExport.pri") {
    error("../../../Build/PlatformExport.pri not found")
}
