!include("RemoteCareController.pri") {
    error("RemoteCareController.pri not found")
}

INCLUDEPATH += ../../../../Common/Components 

TARGET = RemoteCareController

HEADERS +=  ../Include/*.h 

SOURCES +=  ../Source/*.cpp 
