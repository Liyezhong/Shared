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
QT += xml \
      network \
      xmlpatterns \
      core


CONFIG += debug_and_release
#CONFIG += build_all

INCLUDEPATH += ../..
#INCLUDEPATH += ../../../../../Platform/Master/Components/
#               ../../../../../Colorado/Shared/Master/Components/
INCLUDEPATH += ../../../../../Colorado/ColoradoMain/Master/Include
INCLUDEPATH += ../../../../../Colorado/ColoradoMain/Master/Components
INCLUDEPATH += ../../../../../Colorado/Shared/Master/Components
DEPENDPATH += ../..
#              ../../../../../Platform/Master/Components/
#              ../../../../../Colorado/Shared/Master/Components/

QMAKE_CXXFLAGS += -Wall

CONFIG(debug, debug|release) {
    CONFIG_SUFFIX = dbg
} else {
    CONFIG_SUFFIX = rel
    DEFINES += QT_NO_DEBUG \
               QT_NO_DEBUG_OUTPUT \
               NDEBUG
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
###################################
### stuff for adding libraries ####
###################################

# this function adds  libraries passed as arguments to the linker and dependency list
defineTest(UseLib){
    Args=$$ARGS
    LIBS += -Wl,--start-group
    THELIBPATH       = $$ARGS/Build/lib_$$CONFIG_SUFFIX
    LIBNAME          = $$basename(Args)
    PRE_TARGETDEPS  += $$THELIBPATH/lib$${LIBNAME}.a
    LIBS            += $$THELIBPATH/lib$${LIBNAME}.a
    LIBS += -Wl,--end-group
    export(LIBS)
    export(PRE_TARGETDEPS)
}
