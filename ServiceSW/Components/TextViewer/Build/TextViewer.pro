!include(TextViewer.pri) {
    error("TextViewer.pri not found")
}

TARGET = TextViewer

HEADERS *= ../Include/*.h   # header file location

SOURCES *= ../Source/*.cpp  # source file location

FORMS += ../Forms/*.ui
