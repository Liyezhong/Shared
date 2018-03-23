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
INCLUDEPATH +=  . \
                ../..

# we depend of own source and library
DEPENDPATH += . \
              ../Include \
              ../Source \
              ../..

# all warnings
QMAKE_CXXFLAGS += -Wall -std=c++0x

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
		LIBS += -lgcov
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


SHARED = ../../../../../Shared
MASTER = ../../../../../Master

defineTest(UseDepLibs)
{
        Args=$$ARGS
        LIBS += -Wl,--start-group
        for(TheLib, Args) {
                THELIBPATH       = $${MASTER}/$$TheLib/Build/lib_$$CONFIG_SUFFIX
                PRE_TARGETDEPS  += $${THELIBPATH}/lib$${TheLib}.a
                LIBS            += $${THELIBPATH}/lib$${TheLib}.a
        }

        PRE_TARGETDEPS  += $${SHARED}/Common/Components/HimalayaDataContainer/Build/lib_$$CONFIG_SUFFIX/libHimalayaDataContainer.a
        LIBS            += $${SHARED}/Common/Components/HimalayaDataContainer/Build/lib_$$CONFIG_SUFFIX/libHimalayaDataContainer.a
        PRE_TARGETDEPS  += $${MASTER}/Components/HimalayaDataManager/Build/lib_$$CONFIG_SUFFIX/libHimalayaDataManager.a
        LIBS            += $${MASTER}/Components/HimalayaDataManager/Build/lib_$$CONFIG_SUFFIX/libHimalayaDataManager.a
        PRE_TARGETDEPS  += $${MASTER}/Components/HimalayaMasterThread/Build/lib_$$CONFIG_SUFFIX/libHimalayaMasterThread.a
        LIBS            += $${MASTER}/Components/HimalayaMasterThread/Build/lib_$$CONFIG_SUFFIX/libHimalayaMasterThread.a
        PRE_TARGETDEPS  += $${MASTER}/Components/HimalayaGuiController/Build/lib_$$CONFIG_SUFFIX/libHimalayaGuiController.a
        LIBS            += $${MASTER}/Components/HimalayaGuiController/Build/lib_$$CONFIG_SUFFIX/libHimalayaGuiController.a

        LIBS += -Wl,--end-group
        export(LIBS)
        export(PRE_TARGETDEPS)
}
