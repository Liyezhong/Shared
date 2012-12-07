!include("TestServicePassword.pri") {
    error("TestServicePassword.pri not found")
}

TARGET = utTestServicePassword

SOURCES +=  TestServicePassword.cpp

UseLibs(Global PasswordManager)
