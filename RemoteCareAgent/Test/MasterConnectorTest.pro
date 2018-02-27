!include("RemoteCareAgentTest.pri") {
    error("RemoteCareAgentTest.pri not found")
}

TARGET = utTestMasterConnector
extlib.commands = make $$ARCH_SET -C $$AGENTEMBEDDED_PATH
QMAKE_EXTRA_TARGETS += extlib
PRE_TARGETDEPS += extlib

SOURCES +=  TestMasterConnector.cpp 

HEADERS +=  TestMasterConnector.h

