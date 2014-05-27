# include pri file from Master/Test
!include("../../../Test/Platform.pri"):error("../../../Test/Platform.pri not found")

EXTRACOMPONENTDIR = ../..

HIMALAYA                =   ../../../../../Himalaya
PLATFORM               =  ../../../../../Platform

HIMALAYA_MAIN     = $${HIMALAYA}/HimalayaMain
SHARED                    =  $${HIMALAYA}/Shared


# this function adds all libraries passed as arguments to the linker and dependency list
defineTest(UseDepLibs)
{
	Args=$$ARGS
	LIBS += -Wl,--start-group
	for(TheLib, Args) {
		THELIBPATH       = $$EXTRACOMPONENTDIR/$$TheLib/Build/lib_$$CONFIG_SUFFIX
		PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheLib}.a
		LIBS            += $$THELIBPATH/lib$${TheLib}.a
	}

	PRE_TARGETDEPS  += $${SHARED}/Master/Components/HimalayaDataContainer/Build/lib_$$CONFIG_SUFFIX/libHimalayaDataContainer.a
	LIBS            += $${SHARED}/Master/Components/HimalayaDataContainer/Build/lib_$$CONFIG_SUFFIX/libHimalayaDataContainer.a
	PRE_TARGETDEPS  +=$${HIMALAYA_MAIN}/Master/Components/HimalayaDataManager/Build/lib_$$CONFIG_SUFFIX/libHimalayaDataManager.a
	LIBS            += $${HIMALAYA_MAIN}/Master/Components/HimalayaDataManager/Build/lib_$$CONFIG_SUFFIX/libHimalayaDataManager.a
	PRE_TARGETDEPS  += $${HIMALAYA_MAIN}/Master/Components/HimalayaMasterThread/Build/lib_$$CONFIG_SUFFIX/libHimalayaMasterThread.a
	LIBS            += $${HIMALAYA_MAIN}/Master/Components/HimalayaMasterThread/Build/lib_$$CONFIG_SUFFIX/libHimalayaMasterThread.a
	PRE_TARGETDEPS  += $${HIMALAYA_MAIN}/Master/Components/HimalayaGuiController/Build/lib_$$CONFIG_SUFFIX/libHimalayaGuiController.a
	LIBS            += $${HIMALAYA_MAIN}/Master/Components/HimalayaGuiController/Build/lib_$$CONFIG_SUFFIX/libHimalayaGuiController.a

	LIBS += -Wl,--end-group
	export(LIBS)
	export(PRE_TARGETDEPS)
}
