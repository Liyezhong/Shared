!include(ExportData.pri):error("ExportData.pri not found")
TARGET = ExportData # functional unit name
HEADERS *= ../Include/*.h # header file location
SOURCES *= ../Source/*.cpp # source file location

QT +=   xml \        
        xmlpatterns

##############################################################


################# list used platform libraries #################
PLATFORM_COMPONENTS_DIR = ../../../../../../../Platform/Master/Components
PLATFORM_COMPONENTS = ImportExport

################################################################


################# list colorado shared libraries #################
COLORADO_SHAREDCOMPONENTS_DIR = ../../../../../../../Colorado/Shared/Master/Components
COLORADO_SHAREDCOMPONENTS = DataManager

###########################################################
###########################################################
###########################################################################################
################# DO NOT TOUCH THIS UNLESS YOU KNOW WHAT YOU ARE DOING!!! #################
###########################################################################################

################# set destination path
DESTDIR = bin_$$CONFIG_SUFFIX

################# edit include path
INCLUDEPATH += $$PLATFORM_COMPONENTS_DIR \
               $$COLORADO_SHAREDCOMPONENTS_DIR \
               ../../../../../../../Platform/Common/Components

################# start group
LIBS += -Wl,--start-group \

################# include platform libraries and set dependencies
for(TheComponent, PLATFORM_COMPONENTS) {
    THELIBPATH       = $$PLATFORM_COMPONENTS_DIR/$${TheComponent}/Build/lib_$$CONFIG_SUFFIX
    PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheComponent}.a
    LIBS            += $$THELIBPATH/lib$${TheComponent}.a
}

################# include colorado shared libraries and set dependencies
for(TheComponent, COLORADO_SHAREDCOMPONENTS) {
    THELIBPATH       = $$COLORADO_SHAREDCOMPONENTS_DIR/$${TheComponent}/Build/lib_$$CONFIG_SUFFIX
    PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheComponent}.a
    LIBS            += $$THELIBPATH/lib$${TheComponent}.a
}

################# end group
LIBS += -Wl,--end-group
