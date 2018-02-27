!include("RemoteCareAgentTest.pri") {
    error("RemoteCareAgentTest.pri not found")
}

TARGET = utTestREServerConnector
extlib.commands = make $$ARCH_SET -C $$AGENTEMBEDDED_PATH
QMAKE_EXTRA_TARGETS += extlib
PRE_TARGETDEPS += extlib

SOURCES +=  TestREServerConnector.cpp \

HEADERS +=  TestREServerConnector.h \

