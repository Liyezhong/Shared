# #########################################
# ## Build RemoteCare Agent Application ###
# #########################################
!include("RemoteCareAgent.pri") {
 error("RemoteCareAgent.pri not found")
}

extlib.commands = make $$ARCH_SET -C $$AGENTEMBEDDED_PATH
QMAKE_EXTRA_TARGETS += extlib
PRE_TARGETDEPS += extlib

#####################################################
###  WARNING:  due to external library dependencies
###            this project file is Linux-specific.
#####################################################

TEMPLATE = app
TARGET = RemoteCareAgent

HEADERS += ../Include/*.h
SOURCES += ../Source/*.cpp

################# set destination path
DESTDIR = bin_$$CONFIG_SUFFIX

# ################ list used platform libraries #################
PLATFORM_COMPONENTS_DIR = $$PLATFORM_COMPONENTS_PATH
PLATFORM_COMPONENTS = NetworkComponents \
                      NetCommands \
                      Global \
                      DataManager \

# ################ start group
LIBS += -Wl,--start-group

# ################ include platform libraries and set dependencies
for(TheComponent, PLATFORM_COMPONENTS) {
    THELIBPATH = $$PLATFORM_COMPONENTS_DIR/$${TheComponent}/Build/lib_$${CONFIG_SUFFIX}
    PRE_TARGETDEPS += $$THELIBPATH/lib$${TheComponent}.a
    LIBS += $$THELIBPATH/lib$${TheComponent}.a
}

LIBS += ../../Himalaya/Shared/Master/Components/HimalayaDataContainer/Build/lib_$${CONFIG_SUFFIX}/libHimalayaDataContainer.a

LIBS += -Wl,--end-group

# ################ end group

# ################ include Axeda Embedded Agent lib paths:
LIBS += -L$$AGENTEMBEDDED_PATH \
        -L$$AGENTEMBEDDED_PATH/lib/lib \
        -L$$AGENTEMBEDDED_PATH/Libsrc/zlib \
        -L$$AGENTEMBEDDED_PATH/Libsrc/expat/xmlparse
# ################ include Axeda Embedded Agent libs:
LIBS += -lAgentEmbedded \
        -lz \
        -lexpat \
        -lcrypto \
        -lssl \
        -ldl
