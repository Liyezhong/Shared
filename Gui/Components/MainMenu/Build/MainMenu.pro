!include(MainMenu.pri) {
    error("MainMenu.pri not found")
}

TARGET = MainMenu              # functional unit name
HEADERS   *= ../Include/*.h    # header file location
SOURCES   *= ../Source/*.cpp   # source file location
FORMS     *= ../Forms/*.ui     # form file location
RESOURCES *= ../Resource/*.qrc # resource file location
