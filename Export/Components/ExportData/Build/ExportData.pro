!include(ExportData.pri):error("ExportData.pri not found")

TARGET = ExportData # functional unit name


SOURCES += ../Source/ExportData.cpp \
           ../Source/Startup.cpp
# header file location
HEADERS += ../Include/ExportData.h \
           ../Include/Startup.h


#QT +=   xml \
#        xmlpatterns
###############################################################

################## list used platform libraries #################
#PLATFORM_COMPONENTS_DIR = ../../../../../../../Platform/Master/Components
#PLATFORM_COMPONENTS = ImportExport \
#                      Global \
#                      DataManager
#################################################################

############################################################
############################################################
############################################################################################
################## DO NOT TOUCH THIS UNLESS YOU KNOW WHAT YOU ARE DOING!!! #################
############################################################################################

################## set destination path
#DESTDIR = bin_$$CONFIG_SUFFIX

################## edit include path
#INCLUDEPATH += $$PLATFORM_COMPONENTS_DIR

################## start group
#LIBS += -Wl,--start-group \

################## include platform libraries and set dependencies
#for(TheComponent, PLATFORM_COMPONENTS) {
#    THELIBPATH       = $$PLATFORM_COMPONENTS_DIR/$${TheComponent}/Build/lib_$$CONFIG_SUFFIX
#    PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheComponent}.a
#    LIBS            += $$THELIBPATH/lib$${TheComponent}.a
#}

################## end group
#LIBS += -Wl,--end-group

