# include pri file from Platform/Export/Build

!include("../../../../Build/Platform.pri") {
    error("../../../../Build/Platform.pri not found")
}
