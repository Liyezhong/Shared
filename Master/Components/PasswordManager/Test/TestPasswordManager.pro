!include("TestPasswordManager.pri") {
    error("TestPasswordManager.pri not found")
}

TARGET = utTestPasswordManager

SOURCES +=  TestPasswordManager.cpp

UseLibs(DataManager Global PasswordManager)
