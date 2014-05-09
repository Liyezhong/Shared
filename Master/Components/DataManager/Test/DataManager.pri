# include pri file from Master/Test
!include("../../../Test/Platform.pri"):error("../../../Test/Platform.pri not found")


INCLUDEPATH += /usr/local/include

PRE_TARGETDEPS  += /usr/local/lib/gmock/libgmock.a
LIBS            += /usr/local/lib/gmock/libgmock.a
PRE_TARGETDEPS  += /usr/local/lib/gtest/libgtest.a
LIBS            += /usr/local/lib/gtest/libgtest.a




EXTRACOMPONENTDIR = ../..

# this function adds all libraries passed as arguments to the linker and dependency list
defineTest(UseDepLibs){
    Args=$$ARGS
    LIBS += -Wl,--start-group
    for(TheLib, Args) {
        THELIBPATH       = $$EXTRACOMPONENTDIR/$$TheLib/Build/lib_$$CONFIG_SUFFIX
        PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheLib}.a
        LIBS            += $$THELIBPATH/lib$${TheLib}.a
    }

    LIBS += -Wl,--end-group
    export(LIBS)
    export(PRE_TARGETDEPS)
}
