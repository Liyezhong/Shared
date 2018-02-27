!include("RemoteCareAgentTest.pri") {
    error("RemoteCareAgentTest.pri not found")
}

TARGET = utTestRCConfigurationWrapper
extlib.commands = make $$ARCH_SET -C $$AGENTEMBEDDED_PATH
QMAKE_EXTRA_TARGETS += extlib
PRE_TARGETDEPS += extlib


SOURCES +=  TestRCConfigurationWrapper.cpp \

HEADERS +=  TestRCConfigurationWrapper.h \

