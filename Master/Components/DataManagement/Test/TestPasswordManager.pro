!include("DataManagement.pri") {
    error("DataManagement.pri not found")
}

TARGET = utTestPasswordManager

SOURCES +=  TestPasswordManager.cpp

UseLibs(DataManagement Global)
