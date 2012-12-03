# include pri file from Master/Build

!include("../../../Build/Platform.pri") {
    error("../../../Build/Platform.pri not found")
}

QT += xmlpatterns

