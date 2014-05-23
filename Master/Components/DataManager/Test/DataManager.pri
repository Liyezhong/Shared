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

	PRE_TARGETDEPS  += ../../../../../Himalaya/Shared/Master/Components/HimalayaDataContainer/Build/lib_$$CONFIG_SUFFIX/libHimalayaDataContainer.a
	LIBS            += ../../../../../Himalaya/Shared/Master/Components/HimalayaDataContainer/Build/lib_$$CONFIG_SUFFIX/libHimalayaDataContainer.a
	PRE_TARGETDEPS  += ../../../../../Himalaya/HimalayaMain/Master/Components/HimalayaDataManager/Build/lib_$$CONFIG_SUFFIX/libHimalayaDataManager.a
	LIBS            += ../../../../../Himalaya/HimalayaMain/Master/Components/HimalayaDataManager/Build/lib_$$CONFIG_SUFFIX/libHimalayaDataManager.a
	PRE_TARGETDEPS  += ../../../../../Himalaya/HimalayaMain/Master/Components/HimalayaMasterThread/Build/lib_$$CONFIG_SUFFIX/libHimalayaMasterThread.a
	LIBS            += ../../../../../Himalaya/HimalayaMain/Master/Components/HimalayaMasterThread/Build/lib_$$CONFIG_SUFFIX/libHimalayaMasterThread.a
	PRE_TARGETDEPS  += ../../../../../Himalaya/HimalayaMain/Master/Components/HimalayaGuiController/Build/lib_$$CONFIG_SUFFIX/libHimalayaGuiController.a
	LIBS            += ../../../../../Himalaya/HimalayaMain/Master/Components/HimalayaGuiController/Build/lib_$$CONFIG_SUFFIX/libHimalayaGuiController.a

	LIBS += -Wl,--end-group
	export(LIBS)
	export(PRE_TARGETDEPS)
}
