!include("RemoteCareAgentTest.pri") {
    error("RemoteCareAgent.pri not found")
}

TARGET = utTestAgentController

extlib.commands = make $$ARCH_SET -C $$AGENTEMBEDDED_PATH
QMAKE_EXTRA_TARGETS += extlib
PRE_TARGETDEPS += extlib

SOURCES +=  TestAgentController.cpp 

HEADERS +=  TestAgentController.h

