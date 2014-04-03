!include(Settings.pri) {
    error("Settings.pri not found")
}

TARGET = Settings

HEADERS *= ../Include/*.h   \ # header file location
           ../../../Include/*.h \

SOURCES *= ../Source/*.cpp  # source file location

FORMS += ../Forms/*.ui
