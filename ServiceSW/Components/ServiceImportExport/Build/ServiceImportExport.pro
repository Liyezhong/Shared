!include(ServiceImportExport.pri) {
    error("ServiceImportExport.pri not found")
}

TARGET = ServiceImportExport

HEADERS *= ../Include/*.h   \ # header file location

SOURCES *= ../Source/*.cpp  # source file location

QT += network
