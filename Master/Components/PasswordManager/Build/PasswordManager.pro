!include("PasswordManager.pri") {
    error("PasswordManager.pri not found")
}

TARGET = PasswordManager

HEADERS +=  ../Include/*.h

SOURCES += ../Source/*.cpp
