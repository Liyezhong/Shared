!include(IENetworkClient.pri) {
    error("IENetworkClient.pri not found")
}

TARGET = IENetworkClient

HEADERS *= ../Include/*.h   \ # header file location
           ../../../Include/*.h \

SOURCES *= ../Source/*.cpp  # source file location
