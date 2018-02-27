#####################################################
###  WARNING:  due to external library dependencies
###            this project file is Linux-specific.
#####################################################

TEMPLATE = lib

CONFIG += static \
          warn_on \
          qt \
          thread \
          rtti

QT += core \
      xml \
      xmlpatterns \
      network

AGENTEMBEDDED_PATH       = ../../Platform/ExternalPackages/Axeda/AgentEmbedded-6.6-502
PLATFORM_COMPONENTS_PATH = ../../Platform/Master/Components
MY_INCLUDES              = ../Include

INCLUDEPATH += $$MY_INCLUDES \
               $$AGENTEMBEDDED_PATH/Include \
               $$AGENTEMBEDDED_PATH/Sysdeps/Unix \
               $$PLATFORM_COMPONENTS_PATH \
               $$PLATFORM_COMPONENTS_PATH/../../..
DEPENDPATH  += $$INCLUDE_PATH

#setting external lib build for specific arch
linux-g++ {
  ARCH_SET="HOST=1 EXTRA_ARCH=-m32"
}

linux-g++-32 {
  ARCH_SET="HOST=1 EXTRA_ARCH=-m32"
}

linux-g++-64 {
  ARCH_SET="HOST=1 EXTRA_ARCH=-m64"
}

# ###############################################################
CONFIG += debug_and_release
CONFIG(debug, debug|release):CONFIG_SUFFIX = dbg
else {
    CONFIG_SUFFIX = rel
    DEFINES += QT_NO_DEBUG \
        QT_NO_DEBUG_OUTPUT \
        NDEBUG
    CONFIG(gcov) {
        QMAKE_CXXFLAGS_RELEASE += -fprofile-arcs -ftest-coverage
        QMAKE_LFLAGS_RELEASE += -fprofile-arcs
        QMAKE_CXXFLAGS_RELEASE -= -O2
        QMAKE_CXXFLAGS_RELEASE += -O0
        LIBS += -lgcov
    }
}

# ################ set Target differencies:
CONFIG(crosstgt) {
    # ## To be able to build Target run qmake as follows:
    # ###### qmake CONFIG+=crosstgt
    CONFIG_SUFFIX = $${CONFIG_SUFFIX}_tgt
    DEFINES += TARGET_BUILD
    ARCH_SET = ""
}

OBJECTS_DIR = obj_$${CONFIG_SUFFIX}
MOC_DIR = moc_$${CONFIG_SUFFIX}
DESTDIR = lib_$${CONFIG_SUFFIX}
