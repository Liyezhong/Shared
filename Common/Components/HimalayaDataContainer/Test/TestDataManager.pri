######################
# common stuff for all components
######################
######################
# common stuff for all components
######################

# we create applications
TEMPLATE = app

# config for test
CONFIG += qtestlib \
          warn_on \
          qt \
          thread \
          rtti

# remove gui dependency and use QCoreApplication for QTTEST_MAIN
QT -= gui

# both debug and release
CONFIG += debug_and_release
#CONFIG += build_all

# we include current directory
INCLUDEPATH += ../.. \
               ../../../../../Shared/Master/Components \
               ../../../../../../../Platform/Master/Components

# we depend of own source and library
DEPENDPATH += . \
              ../.. \
              ../../../../../Shared/Master/Components \
              ../../../../../../../Platform/Master/Components

# compute CONFIG_SUFFIX depending on debug / release
CONFIG(debug, debug|release) {
    CONFIG_SUFFIX = dbg
} else {
    CONFIG_SUFFIX = rel
    CONFIG(gcov) {
        QMAKE_CXXFLAGS_RELEASE += -fprofile-arcs -ftest-coverage
        QMAKE_LFLAGS_RELEASE += -fprofile-arcs
        QMAKE_CXXFLAGS_RELEASE -= -O2
        QMAKE_CXXFLAGS_RELEASE += -O0
    }
}

# set compile output directories
OBJECTS_DIR = obj_$$CONFIG_SUFFIX
MOC_DIR = moc_$$CONFIG_SUFFIX
DESTDIR = bin_$$CONFIG_SUFFIX

###################################
### stuff for adding libraries ####
###################################

# this function adds  libraries passed as arguments to the linker and dependency list
defineTest(UseLib){
    Args=$$ARGS
    LIBS += -Wl,--start-group
    for(TheLib, Args) {
        THELIBPATH       = $$TheLib/Build/lib_$$CONFIG_SUFFIX
        LIBNAME          = $$basename(TheLib)
        PRE_TARGETDEPS  += $$THELIBPATH/lib$${LIBNAME}.a
        LIBS            += $$THELIBPATH/lib$${LIBNAME}.a
    }
    LIBS += -Wl,--end-group
    export(LIBS)
    export(PRE_TARGETDEPS)
}




