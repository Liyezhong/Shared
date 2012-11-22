# #########################################
# ## Build RemoteCare Agent Application ###
# #########################################
# !include("RemoteCareAgent.pri") {
# error("RemoteCareAgent.pri not found")
# }

#####################################################
###  WARNING:  due to external library dependencies
###            this project file is Linux-specific.
#####################################################

TEMPLATE = app
TARGET = RemoteCareAgent
CONFIG -= static
CONFIG += warn_on \
          qt \
          thread \
          rtti

QT -= gui
QT += core \
      xml \
      xmlpatterns \
      network

AGENTEMBEDDED_PATH = ../../../ExternalPackages/Axeda/AgentEmbedded-6.1-355
PLATFORM_COMPONENTS_PATH = ../../../Components

HEADERS += ../Include/*.h \
           ../Include/Commands/*.h \
           ../../../ExternalPackages/Axeda/AgentEmbedded-6.1-355/Include \
           ../../../ExternalPackages/Axeda/AgentEmbedded-6.1-355/Sysdeps/Unix
SOURCES += ../Source/*.cpp \
           ../Source/Commands/*.cpp

INCLUDEPATH += $$HEADERS \
               $$PLATFORM_COMPONENTS_PATH
DEPENDPATH  += $$HEADERS \
               $$PLATFORM_COMPONENTS_PATH

# ################ list used platform libraries #################
PLATFORM_COMPONENTS_DIR = $$PLATFORM_COMPONENTS_PATH
PLATFORM_COMPONENTS = NetworkComponents \
                      Global

# ###############################################################
CONFIG += debug_and_release
CONFIG(debug, debug|release):CONFIG_SUFFIX = dbg
else {
    CONFIG_SUFFIX = rel
    DEFINES += QT_NO_DEBUG \
        QT_NO_DEBUG_OUTPUT \
        NDEBUG
    CONFIG(gcov) {
        QMAKE_CXXFLAGS_RELEASE += -fprofile-arcs \
            -ftest-coverage
        QMAKE_LFLAGS_RELEASE += -fprofile-arcs
        QMAKE_CXXFLAGS_RELEASE -= -O2
        QMAKE_CXXFLAGS_RELEASE += -O0
    }
}

# ################ include Axeda Embedded Agent lib paths:
LIBS += -L$$AGENTEMBEDDED_PATH \
        -L$$AGENTEMBEDDED_PATH/lib/lib \
        -L/usr/lib  # for libssl on Linux Host
# ################ include Axeda Embedded Agent libs:
LIBS += -lz \
        -lexpat \
        -lssl \
        -lAgentEmbedded
# ################ set Target differencies:
CONFIG(crosstgt) {
    # ## To be able to build Target run qmake as follows:
    # ###### qmake CONFIG+=crosstgt
    CONFIG_SUFFIX = $${CONFIG_SUFFIX}_tgt
    DEFINES += TARGET_BUILD
    # take target's libssl:
    LIBS -= -L/usr/lib
    LIBS += -L/opt/OSELAS.BSP-Phytec-phyCORE-i.MX35-PD10.1.2/platform-phyCORE-i.MX35/sysroot-target/usr/lib
}
OBJECTS_DIR = obj_$${CONFIG_SUFFIX}
MOC_DIR = moc_$${CONFIG_SUFFIX}
DESTDIR = bin_$${CONFIG_SUFFIX}

# ################ start group
LIBS += -Wl,--start-group

# ################ include platform libraries and set dependencies
for(TheComponent, PLATFORM_COMPONENTS) {
    THELIBPATH = $$PLATFORM_COMPONENTS_DIR/$${TheComponent}/Build/lib_$${CONFIG_SUFFIX}
    PRE_TARGETDEPS += $$THELIBPATH/lib$${TheComponent}.a
    LIBS += $$THELIBPATH/lib$${TheComponent}.a
}
LIBS += -Wl,--end-group

# ################ end group
