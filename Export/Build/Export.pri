######################
# common stuff for all components
######################

TEMPLATE = lib

CONFIG += static \
          warn_on \
          qt \
          thread \
          rtti

QT -= gui
QT += core \
      xml \
      xmlpatterns \
      network

INCLUDEPATH += ../.. \
               ../../../../../Platform/Master/Components \

DEPENDPATH +=  ../.. \
               ../../../../../Platform/Master/Components


CONFIG += debug_and_release
#CONFIG += build_all

QMAKE_CXXFLAGS += -Wall

CONFIG(debug, debug|release) {
    CONFIG_SUFFIX = dbg
} else {
    CONFIG_SUFFIX = rel
    DEFINES += QT_NO_DEBUG \
               QT_NO_DEBUG_OUTPUT \
                           DBT_TRACE_DISCARD \
               NDEBUG
    CONFIG(gcov) {
        QMAKE_CXXFLAGS_RELEASE += -fprofile-arcs -ftest-coverage
        QMAKE_LFLAGS_RELEASE += -fprofile-arcs
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
