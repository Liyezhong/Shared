!include("NetworkComponents.pri") {
    error("NetworkComponents.pri not found")
}

TARGET = NetworkComponents

HEADERS +=  ../Include/*.h \
            ../Include/ProtocolRxCommands/*.h \
            ../Include/ProtocolTxCommands/*.h

SOURCES +=  ../Source/*.cpp \
            ../Source/ProtocolRxCommands/*.cpp \
            ../Source/ProtocolTxCommands/*.cpp
