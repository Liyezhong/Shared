# include pri file from Master/Test

!include("../../../Test/Platform.pri") {
    error("../../../Test/Platform.pri not found")
}

QT += xml network
