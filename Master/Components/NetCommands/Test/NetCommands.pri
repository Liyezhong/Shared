# include pri file from Master/Test

!include("../../../Test/Shared.pri") {
    error("../../../Test/Shared.pri not found")
}

QT += xml
