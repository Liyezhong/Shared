# include pri file from Master/Build

!include("../../../Test/Platform.pri") {
    error("../../../Test/Platform.pri not found")
}
