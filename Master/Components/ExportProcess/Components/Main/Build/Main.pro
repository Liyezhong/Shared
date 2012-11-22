################# include pri file #################
!include("Main.pri") {
    error("Main.pri not found")
}
####################################################



################# override some pri settings #################
TEMPLATE = app
TARGET = PlatformExport
HEADERS *= ../Include/*.h # header file location
SOURCES *= ../Source/*.cpp # source file location
CONFIG -= static
QT +=   xml \        
        xmlpatterns
##############################################################

################# list used Export libraries #################
PLATFORM_EXPORT_COMPONENTS_DIR = ../../../../../../../Platform/Master/Components/ExportProcess/Components
PLATFORM_EXPORT_COMPONENTS = ExportData
################################################################

################# list used platform libraries #################
PLATFORM_COMPONENTS_DIR = ../../../../../../../Platform/Master/Components
PLATFORM_COMPONENTS = ImportExport \
                      Global
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
               $$PLATFORM_EXPORT_COMPONENTS_DIR \
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

################# include platform export libraries and set dependencies
for(TheComponent, PLATFORM_EXPORT_COMPONENTS) {
    THELIBPATH       = $$PLATFORM_EXPORT_COMPONENTS_DIR/$${TheComponent}/Build/bin_$$CONFIG_SUFFIX
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
