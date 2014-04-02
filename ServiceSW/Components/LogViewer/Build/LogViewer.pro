!include(LogViewer.pri) {
    error("LogViewer.pri not found")
}

TARGET = LogViewer

HEADERS *= ../Include/*.h   # header file location

SOURCES *= ../Source/*.cpp  # source file location

FORMS += ../Forms/*.ui
