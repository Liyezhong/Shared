# include pri file from Shared/Build

#!include("../../../Build/Shared.pri") {
#    error("../../../Build/Shared.pri not found")
#}

!include("../../../Build/Platform.pri") {
    error("../../../Build/Platform.pri not found")
}

