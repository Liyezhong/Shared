!include("PasswordManager.pri") {
    error("PasswordManager.pri not found")
}

TARGET = utTestPasswordManager

SOURCES +=  TestPasswordManager.cpp

UseLibs(DataManager Global)
