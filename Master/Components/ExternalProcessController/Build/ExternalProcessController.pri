# include pri file from Platform/Build

!include("../../../Build/Platform.pri") {
    error("../../../Build/Platform.pri not found")
}

QT += core \
      xmlpatterns \
      network
