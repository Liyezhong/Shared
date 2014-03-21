# include pri file from Platform/Export/Build

!include("../../../Build/Export.pri") {
    error("../../../Build/Export.pri not found")
}
