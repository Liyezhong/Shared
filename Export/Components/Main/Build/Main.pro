################# include pri file #################
!include("Main.pri") {
    error("Main.pri not found")
}
####################################################


################# override some pri settings #################
TEMPLATE = app
TARGET = Export

HEADERS += ../Include/Main.h # header file location
SOURCES += ../Source/Main.cpp # source file location
CONFIG -= static
QT +=   xml \        
        xmlpatterns
##############################################################

################# list used Export libraries #################
PLATFORM_EXPORT_COMPONENTS_DIR = ../../../../Export/Components
PLATFORM_EXPORT_COMPONENTS = ExportData
################################################################

################# list used platform libraries #################
PLATFORM_COMPONENTS_DIR = ../../../../Master/Components
PLATFORM_COMPONENTS = EncryptionDecryption \
                      Global \
                      DataManager
################################################################

###########################################################
###########################################################
###########################################################################################
################# DO NOT TOUCH THIS UNLESS YOU KNOW WHAT YOU ARE DOING!!! #################
###########################################################################################

################# set destination path
DESTDIR = bin_$$CONFIG_SUFFIX

################# edit include path
INCLUDEPATH += $$PLATFORM_COMPONENTS_DIR \
               $$PLATFORM_EXPORT_COMPONENTS_DIR

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
    THELIBPATH       = $$PLATFORM_EXPORT_COMPONENTS_DIR/$${TheComponent}/Build/lib_$$CONFIG_SUFFIX
    PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheComponent}.a
    LIBS            += $$THELIBPATH/lib$${TheComponent}.a
}


################# end group
LIBS += -Wl,--end-group
