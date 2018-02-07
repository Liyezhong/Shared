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
               ../../../../../../Platform/Master/Components

# we depend of own source and library
DEPENDPATH += . \
              ../Include \
              ../Source \
              ../.. \
              ../../../../../../Platform/Master/Components

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
COMPONENTDIR = ../..
COMPONENTDIRPL = ../../../../../../Platform/Master/Components

# this function adds all libraries passed as arguments to the linker and dependency list
defineTest(UseLibs){
    Args=$$ARGS
    LIBS += -Wl,--start-group
    for(TheLib, Args) {
        THELIBPATH       = $$COMPONENTDIR/$$TheLib/Build/lib_$$CONFIG_SUFFIX
        PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheLib}.a
        LIBS            += $$THELIBPATH/lib$${TheLib}.a
    }
    LIBS += -Wl,--end-group
    export(LIBS)
    export(PRE_TARGETDEPS)
}
###################################

# this function adds all libraries passed as arguments to the linker and dependency list
defineTest(UseLibsPlatform){
    Args=$$ARGS
    LIBS += -Wl,--start-group
    for(TheLib, Args) {
        THELIBPATH       = $$COMPONENTDIRPL/$$TheLib/Build/lib_$$CONFIG_SUFFIX
        PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheLib}.a
        LIBS            += $$THELIBPATH/lib$${TheLib}.a
    }
    LIBS += -Wl,--end-group
    export(LIBS)
    export(PRE_TARGETDEPS)
}
###################################

