!include(Application.pri):error("Application.pri not found")
TARGET = Application # functional unit name
HEADERS *= ../Include/*.h # header file location
SOURCES *= ../Source/*.cpp # source file location
RESOURCES += ../Resource/Application.qrc
