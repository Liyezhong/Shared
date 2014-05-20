!include(ServiceKeyValidator.pri) {
    error("ServiceKeyValidator.pri not found")
}

TARGET = ServiceKeyValidator

HEADERS *= ../Include/*.h   \ # header file location
           ../../../Include/*.h \

SOURCES *= ../Source/*.cpp  # source file location
