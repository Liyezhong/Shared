######################
# common stuff for all components
######################

TEMPLATE = lib

CONFIG += static \
          warn_on \
          qt \
          thread \
          rtti

QT += gui \
      core \


CONFIG += debug_and_release
#CONFIG += build_all
CONFIG += uitools

INCLUDEPATH += ../.. \
               ../../../../../Platform/Master/Components \
               ../../../../../Himalaya/HimalayaMain/Gui/Components \
               ../../../../../Himalaya/Shared/Gui/Components

DEPENDPATH += ../.. \
              ../../../../../Platform/Master/Components \
              ../../../../../Himalaya/HimalayaMain/Gui/Components \
              ../../../../../Himalaya/Shared/Gui/Components

QMAKE_CXXFLAGS += -Wall -std=c++0x

CONFIG(debug, debug|release) {
    CONFIG_SUFFIX = dbg
    QMAKE_LFLAGS_DEBUG = -rdynamic
} else {
    CONFIG_SUFFIX = rel
    DEFINES += QT_NO_DEBUG \
               QT_NO_DEBUG_OUTPUT \
               NDEBUG
    QMAKE_CXXFLAGS_RELEASE += -g
    CONFIG(gcov) {
        QMAKE_CXXFLAGS_RELEASE += -fprofile-arcs -ftest-coverage
        QMAKE_CXXFLAGS_RELEASE -= -O2
        QMAKE_CXXFLAGS_RELEASE += -O0
    }
}

CONFIG(crosstgt) {
### To be able to build Target run qmake as follows:
####### qmake CONFIG+=crosstgt
    CONFIG_SUFFIX = $${CONFIG_SUFFIX}_tgt
    DEFINES += TARGET_BUILD
}

OBJECTS_DIR = obj_$${CONFIG_SUFFIX}
MOC_DIR = moc_$${CONFIG_SUFFIX}
DESTDIR = lib_$${CONFIG_SUFFIX}

######################
