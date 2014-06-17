#!/bin/bash

#****************************************************************************/
 #! \file Include.sh
 #
 #  \brief  Contains environment for SWUpdate -Globals/constants/EventStrings
 #
 #   $Version: $ 1.1
 #   $Date:    $ 2014-03-13
 #   $Author:  $ Hemanth Kumar, Nikhil Kamath
 #
 #  \b Company:
 #
 #       Leica Biosystems Nussloch GmbH.
 #
 #  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 #  This is unpublished proprietary source code of Leica. The copyright notice
 #  does not evidence any actual or intended publication.
 #
 #/
#****************************************************************************/


readonly ROOTDIR="/home/Leica/"					# Path of Root Directory 
if [ ! -d $ROOTDIR ]; then
	echo "$ROOTDIR does not exist"
	exit 1
fi


readonly BINDIR="$ROOTDIR/Bin"				    # Path of SW binaries
readonly FIRMWAREDIR="$ROOTDIR/Firmware"		# Path of Firmware binaries
readonly LOGDIR="$ROOTDIR/Logfiles"				# Path of Log folder
readonly ROLLBACKDIR="$ROOTDIR/Rollback"		# Path of Rollback folder 
readonly SETTINGDIR="$ROOTDIR/Settings"			# Path of current configuration settings folder
readonly TRANSLATIONSDIR="$ROOTDIR/Translations" #main software & GUI translations
readonly TRANSLATIONSSERVDIR="$ROOTDIR/TranslationsService" #service software translations
readonly STARTDIR="$ROOTDIR/StartUp"			# Path of StartUp script
readonly SCRIPTSDIR="$ROOTDIR/Scripts"          # Path of init & helper scripts 
readonly EBOXINITSCRIPTSDIR="/etc/init.d"       # Path primitive start up scripts 
readonly TMPDIR="$ROOTDIR/tmp"				    # Temporary folder
readonly UPDATEDIR="$TMPDIR/MasterSWUpdate"		# Path to Master SW Update directory		
readonly UPDATEPKGDIR="$UPDATEDIR/UpdatePackage"		# Path to update package
readonly TMPSETTINGDIR="$UPDATEPKGDIR/Settings" 		# Temporary settings folder
readonly TMPBINDIR="$UPDATEPKGDIR/Bin"				# Temporary SW binary folder
readonly TMPFIRMWAREDIR="$UPDATEPKGDIR/Firmware"		# Temporary Firmware binary
readonly TMPTRANSLATIONDIR="$UPDATEPKGDIR/Translations"		# Temporary main software translations
readonly TMPTRANSLATIONSSERVICEDIR="$UPDATEPKGDIR/TranslationsService"		# Temporary Service translations
readonly TMPSCRIPTS="$UPDATEPKGDIR/Scripts"             #Update package Scripts folder     
readonly TMPINITD="$UPDATEPKGDIR/init.d"                #Update package init.d folder
readonly SLAVEFILEDIR="$UPDATEDIR/SlaveSWUpdateFiles"	# Path to slave SW update files
readonly MOUNTPOINT="/mnt/mnt_storage" 					    # USB Mount point
readonly REMOTECAREPATH="$ROOTDIR/RemoteCare"  			# RemoteCare Package path
readonly ROLLSWVERFILE="$ROLLBACKDIR/Settings/SW_Version.xml" 	# Path of SW Version file in Rollback folder 
readonly HWVERFILE="$SETTINGDIR/Slave_HW_Version.txt"		 # HW_Version file present on device
if [ -f $HWVERFILE ]; then
	NAME=$(head -1  $HWVERFILE)
else
	echo "$HWVERFILE file missing"				# Since Log file does not exist at this point 
	exit 1							# so echoing error message to console	
fi


readonly UPDATEHWVERFILE="$TMPSETTINGDIR/Slave_HW_Version.txt" # HW_Version file in SW Package
readonly SWVERFILE="$SETTINGDIR/SW_Version.xml"			 # SW_Version.xml file present on device
readonly TMPSWVERSIONFILE=$TMPDIR/temp_SW_Version.xml		 # Temporary file used for intermediate processing
readonly UPDATESWVERFILE="$TMPSETTINGDIR/SW_Version.xml"	 # SW_Version.xml file in SW package
readonly MD5FILE="$UPDATEPKGDIR/md5sum.txt"		# md5 location in package directory
readonly TMPDIRMD5FILE="$TMPDIR/md5sum.txt"		# md5 file
readonly ROLLBACKBINDIR="$ROLLBACKDIR/Bin"		 	# Rollback bin directory
readonly ROLLBACKFIRMWAREDIR="$ROLLBACKDIR/Firmware"	# Rollback firmware directory
readonly ROLLBACKSETTINGSDIR="$ROLLBACKDIR/Settings"   #Rollback settings directory
readonly ROLLBACKSCRIPTSDIR="$ROLLBACKDIR/Scripts" # Parent Directory for init scripts rollback
readonly ROLLBACKINITSCRIPTS="$ROLLBACKDIR/init.d" # Primitive init scripts rollback
readonly ROLLBACKTRANSLATIONS="$ROLLBACKDIR/Translations" # rollback for translations
readonly ROLLBACKTRANSLATIONSSERV="$ROLLBACKDIR/TranslationsService" # rollback for translations
readonly PTSFILE="$TMPBINDIR/prototest"		 		# PTS file
readonly SLAVEUPDATEFILE="$TMPBINDIR/SlaveUpdate.js"	# Slave update java script file
readonly BOOTCONFIGFILE="$SETTINGDIR/BootConfig.txt"		# BootConfig.txt file present on device
readonly POWERFAILSCRIPT="PowerFailMonitor.sh"  #Power Fail Monitoring script
readonly BIN="Bin"				# Path of bin directory folder - used for md5checksum
readonly FIRMWARE="Firmware"				# Path of Firmware directory folder 
readonly SETTING="Settings"				# Path of Settings directory folder 
readonly TRANSLATIONS="Translations"    # Path of Translations directory folder
readonly TRANSLATIONSSERVICE="TranslationsService" #Path for service translations
readonly LIST_OF_DIR_FOR_MD5SUM_CHECK=( "$BIN"\
                                        "$FIRMWARE"\
                                        "$SETTING"\
                                        "$TRANSLATIONS"\
                                        "$TRANSLATIONSSERVICE"\
                                        "Scripts"\
                                        "init.d"
                                      )
FILE_IGNORE_LIST=() #List of files to ignore while updating Settings directory
DIR_IGNORE_LIST=()  #Directories in Settings to be ignored while updating
UPDATED_SW_BINARIES=() #Names of updated SW binaries
UPDATED_FIRMWARE=()    #Names of  updated firmware
FIRMWARES_TO_BE_UPDATED=() #Names of firmware to be updated
readonly LOGFILE="SW_Update_Events.log"		 # Log file
readonly PKGFOOTER="_SW_Update_Package_"			 # Package name footer
readonly EVENT_SOURCE_MASTER="Master SW Update"		 # Source event type
readonly EVENT_SOURCE_SLAVE="Slave SW Update"		 # Source event type


# to declare associative arrays it is require to use -A option
readonly EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED=1
readonly EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS=2
readonly EVENT_SWUPDATE_INVALID_CMD_ARGS=3
readonly EVENT_SWUPDATE_MD5SUM_CHK_FAILED=4
readonly EVENT_SWUPDATE_CRC_CHK_FAILED=5
readonly EVENT_SWUPDATE_SLAVE_DOESNOT_HAVE_REQ_HW_VER=6
readonly EVENT_SWUPDATE_NO_USB_DEV_FOUND=7
readonly EVENT_SWUPDATE_UPDATE_PACKAGE_NOT_FOUND=8
readonly EVENT_SWUPDATE_FILE_FOLDER_CREATION_FAILED=9
readonly EVENT_SWUPDATE_SLAVE_UPDATE_FAILED=10
readonly EVENT_SWUPDATE_FILE_FOLDER_DELETION_FAILED=11
readonly EVENT_SWUPDATE_MISSING_SLAVE=12	
readonly EVENT_SWUPDATE_COPYING_FILE=13
readonly EVENT_SWUPDATE_MISSING_XML_ATTR=14	
readonly EVENT_SWUPDATE_MOUNTING_DEV_PASSED=15
readonly EVENT_SWUPDATE_UNZIP_HIGHEST_VER_UPDATE_PACKAGE=16	
readonly EVENT_SWUPDATE_CRC_CHK_PASSED=17
readonly EVENT_SWUPDATE_MD5SUM_CHK_PASSED=18
readonly EVENT_SWUPDATE_SLAVE_HAS_REQ_HW_VER=19
readonly EVENT_SWUPDATE_FILE_FOLDER_CREATION=20
readonly EVENT_SWUPDATE_EXTRACTING_PACKAGE=21
readonly EVENT_SWUPDATE_EXECUTING_COMPONENT=22
readonly EVENT_SWUPDATE_ROLLINGBACK=23
readonly EVENT_SWUPDATE_PARSING_XML=24
readonly EVENT_SWUPDATE_SUCCESS=25
readonly EVENT_SWUPDATE_MOUNTING_DEV_FAILED=26
readonly EVENT_SWUPDATE_SWVERSION_FILE_NOT_FOUND=27
readonly EVENT_SWUPDATE_SWVERSION_FILE_CORRUPTED=28
readonly EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED=29
readonly EVENT_SWUPDATE_STARTER_SUCCESS=30
readonly EVENT_SWUPDATE_PTS_SUCCESS=31
readonly EVENT_SWUPDATE_PTS_FAILED=32
readonly EVENT_SWUPDATE_USB_UNMOUNTED=33
readonly EVENT_SWUPDATE_BOOTCONFIG_FILE_NOT_FOUND=34
readonly EVENT_SWUPDATE_BOOTCONFIG_FILE_UPDATE_SUCCESS=35
readonly EVENT_SWUPDATE_BOOTCONFIG_UPDATE_ENTRY_MISSING=36
readonly EVENT_SWUPDATE_UPDATING_FILE=37
readonly EVENT_SWUPDATE_POWERFAILED=38
readonly EVENT_SWUPDATE_ROLLBACK_SWVERSION=39
readonly EVENT_SWUPDATE_FAILED=40
readonly EVENT_SWUPDATE_REMOVE_OBSOLETE_FILE=41
readonly EVENT_SWUPDATE_CONFIG_FILE_MODIFIED=42
readonly EVENT_SWUPDATE_XML_NODE_ERROR=43
readonly EVENT_SWUPDATE_ROLLBACKFAILED=44
readonly EVENT_SWUPDATE_ERROR_MD5SUM=45
readonly EVENT_SWUPDATE_CHECK_SUCCESS=46

# declaring the associate arrays
declare -A EventString
declare -A EventLevel
declare -A EventType

# assigning the strings to associate errors
EventString[$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED]="SWUpdate Failed:File/Folder copy failed from %s to %s"
EventString[$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS]="SWUpdate Failed:File/Folder %s does not exists"
EventString[$EVENT_SWUPDATE_INVALID_CMD_ARGS]="SWUpdate Failed:Invalid Command line arguments:%s"
EventString[$EVENT_SWUPDATE_MD5SUM_CHK_FAILED]="SWUpdate Failed:Md5sum check failed:%s"
EventString[$EVENT_SWUPDATE_CRC_CHK_FAILED]="SWUpdate Failed:CRC check failed"
EventString[$EVENT_SWUPDATE_SLAVE_DOESNOT_HAVE_REQ_HW_VER]="SWUpdate Failed:Slave %s has HW version %s .Expected HW Version %s ."
EventString[$EVENT_SWUPDATE_NO_USB_DEV_FOUND]="SWUpdate Failed:No USB Device found"
EventString[$EVENT_SWUPDATE_UPDATE_PACKAGE_NOT_FOUND]="SWUpdate Failed:No Update Package found at %s"
EventString[$EVENT_SWUPDATE_FILE_FOLDER_CREATION_FAILED]="SWUpdate Failed:File/Folder creation failed for %s"
EventString[$EVENT_SWUPDATE_SLAVE_UPDATE_FAILED]="SWUpdate Failed: Slave SW update failed"
EventString[$EVENT_SWUPDATE_FILE_FOLDER_DELETION_FAILED]="SWUpdate Failed:File/Folder deletion failed for %s"
EventString[$EVENT_SWUPDATE_MISSING_SLAVE]="SWUpdate Failed:Missing slave name %s in %s"
EventString[$EVENT_SWUPDATE_COPYING_FILE]="Copying the File/Folder from %s to %s"
EventString[$EVENT_SWUPDATE_MISSING_XML_ATTR]="XML attribute %s found in %s but not in %s"
EventString[$EVENT_SWUPDATE_MOUNTING_DEV_PASSED]="Mounted USB device to path %s"
EventString[$EVENT_SWUPDATE_UNZIP_HIGHEST_VER_UPDATE_PACKAGE]="Unzipping the update package %s to %s"
EventString[$EVENT_SWUPDATE_CRC_CHK_PASSED]="CRC check passed"
EventString[$EVENT_SWUPDATE_MD5SUM_CHK_PASSED]="Md5sum check passed"
EventString[$EVENT_SWUPDATE_SLAVE_HAS_REQ_HW_VER]="Current HW version of %s is %s"
EventString[$EVENT_SWUPDATE_FILE_FOLDER_CREATION]="Creating File/Folder %s"
EventString[$EVENT_SWUPDATE_EXTRACTING_PACKAGE]="Extracting package from %s"
EventString[$EVENT_SWUPDATE_EXECUTING_COMPONENT]="Executing the script %s"
EventString[$EVENT_SWUPDATE_ROLLINGBACK]="Rolling back the files in %s from %s"
EventString[$EVENT_SWUPDATE_PARSING_XML]="Parsing the XML file %s"
EventString[$EVENT_SWUPDATE_SUCCESS]="SWUpdate Success"
EventString[$EVENT_SWUPDATE_MOUNTING_DEV_FAILED]="Mounting USB Device failed"
EventString[$EVENT_SWUPDATE_SWVERSION_FILE_NOT_FOUND]="%s file not found. Updating all SW binaries and firmwares"
EventString[$EVENT_SWUPDATE_SWVERSION_FILE_CORRUPTED]="%s file corrupted. Updating all SW binaries and firmwares"
EventString[$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED]="SWUpdate Failed:%s file corrupted."
EventString[$EVENT_SWUPDATE_STARTER_SUCCESS]="Execution of the starter succeeded"
EventString[$EVENT_SWUPDATE_PTS_SUCCESS]="PTS execution is success"
EventString[$EVENT_SWUPDATE_PTS_FAILED]="PTS execution is failed.Exit code %s"
EventString[$EVENT_SWUPDATE_USB_UNMOUNTED]="Unmounted USB from %s"
EventString[$EVENT_SWUPDATE_BOOTCONFIG_FILE_NOT_FOUND]="%s file not found"
EventString[$EVENT_SWUPDATE_BOOTCONFIG_FILE_UPDATE_SUCCESS]="%s file updated"
EventString[$EVENT_SWUPDATE_BOOTCONFIG_UPDATE_ENTRY_MISSING]="%s file missing entry of 'Software_Update_Status'"
EventString[$EVENT_SWUPDATE_UPDATING_FILE]="Updating %s from version %s to version %s"
EventString[$EVENT_SWUPDATE_POWERFAILED]="SWUpdate interrupted due to Power Failure. SW Update Failed"
EventString[$EVENT_SWUPDATE_ROLLBACK_SWVERSION]="Rolling back file %s with %s"
EventString[$EVENT_SWUPDATE_FAILED]="SWUpdate:Update Failed"
EventString[$EVENT_SWUPDATE_REMOVE_OBSOLETE_FILE]="Removing obsolete file %s"
EventString[$EVENT_SWUPDATE_CONFIG_FILE_MODIFIED]="Updating Readonly file/folder %s"
EventString[$EVENT_SWUPDATE_XML_NODE_ERROR]="SW_Version.xml xml node mismatch"
EventString[$EVENT_SWUPDATE_ROLLBACKFAILED]="Rolling back %s failed"
EventString[$EVENT_SWUPDATE_ERROR_MD5SUM]="Updating/Verfying Md5sum failed for %s"
EventString[$EVENT_SWUPDATE_CHECK_SUCCESS]="Software Update package check success"

# assigning the event level to associate array
EventLevel[$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED]="High"
EventLevel[$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS]="High"
EventLevel[$EVENT_SWUPDATE_INVALID_CMD_ARGS]="High"
EventLevel[$EVENT_SWUPDATE_MD5SUM_CHK_FAILED]="High"
EventLevel[$EVENT_SWUPDATE_CRC_CHK_FAILED]="High"
EventLevel[$EVENT_SWUPDATE_SLAVE_DOESNOT_HAVE_REQ_HW_VER]="High"
EventLevel[$EVENT_SWUPDATE_NO_USB_DEV_FOUND]="High"
EventLevel[$EVENT_SWUPDATE_UPDATE_PACKAGE_NOT_FOUND]="High"
EventLevel[$EVENT_SWUPDATE_FILE_FOLDER_CREATION_FAILED]="High"
EventLevel[$EVENT_SWUPDATE_SLAVE_UPDATE_FAILED]="High"
EventLevel[$EVENT_SWUPDATE_FILE_FOLDER_DELETION_FAILED]="High"
EventLevel[$EVENT_SWUPDATE_MISSING_SLAVE]="High"
EventLevel[$EVENT_SWUPDATE_COPYING_FILE]="Low"
EventLevel[$EVENT_SWUPDATE_MISSING_XML_ATTR]="Low"
EventLevel[$EVENT_SWUPDATE_MOUNTING_DEV_PASSED]="Low"
EventLevel[$EVENT_SWUPDATE_UNZIP_HIGHEST_VER_UPDATE_PACKAGE]="Low"
EventLevel[$EVENT_SWUPDATE_CRC_CHK_PASSED]="Low"
EventLevel[$EVENT_SWUPDATE_MD5SUM_CHK_PASSED]="Low"
EventLevel[$EVENT_SWUPDATE_SLAVE_HAS_REQ_HW_VER]="Low"
EventLevel[$EVENT_SWUPDATE_FILE_FOLDER_CREATION]="Low"
EventLevel[$EVENT_SWUPDATE_EXTRACTING_PACKAGE]="Low"
EventLevel[$EVENT_SWUPDATE_EXECUTING_COMPONENT]="Low"
EventLevel[$EVENT_SWUPDATE_ROLLINGBACK]="Low"
EventLevel[$EVENT_SWUPDATE_PARSING_XML]="Low"
EventLevel[$EVENT_SWUPDATE_SUCCESS]="High"
EventLevel[$EVENT_SWUPDATE_MOUNTING_DEV_FAILED]="Low"
EventLevel[$EVENT_SWUPDATE_SWVERSION_FILE_NOT_FOUND]="Low"
EventLevel[$EVENT_SWUPDATE_SWVERSION_FILE_CORRUPTED]="Low"
EventLevel[$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED]="High"
EventLevel[$EVENT_SWUPDATE_STARTER_SUCCESS]="High"
EventLevel[$EVENT_SWUPDATE_PTS_SUCCESS]="Low"
EventLevel[$EVENT_SWUPDATE_PTS_FAILED]="Low"
EventLevel[$EVENT_SWUPDATE_USB_UNMOUNTED]="Low"
EventLevel[$EVENT_SWUPDATE_BOOTCONFIG_FILE_NOT_FOUND]="Low"
EventLevel[$EVENT_SWUPDATE_BOOTCONFIG_FILE_UPDATE_SUCCESS]="Low"
EventLevel[$EVENT_SWUPDATE_BOOTCONFIG_UPDATE_ENTRY_MISSING]="Low"
EventLevel[$EVENT_SWUPDATE_UPDATING_FILE]="High"
EventLevel[$EVENT_SWUPDATE_POWERFAILED]="High"
EventLevel[$EVENT_SWUPDATE_ROLLBACK_SWVERSION]="High"
EventLevel[$EVENT_SWUPDATE_FAILED]="High"
EventLevel[$EVENT_SWUPDATE_REMOVE_OBSOLETE_FILE]="High"
EventLevel[$EVENT_SWUPDATE_CONFIG_FILE_MODIFIED]="High"
EventLevel[$EVENT_SWUPDATE_XML_NODE_ERROR]="High"
EventLevel[$EVENT_SWUPDATE_ROLLBACKFAILED]="High"
EventLevel[$EVENT_SWUPDATE_ERROR_MD5SUM]="High"
EventLevel[$EVENT_SWUPDATE_CHECK_SUCCESS]="High"

# assigning the event type to associate array
EventType[$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED]="Error"
EventType[$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS]="Error"
EventType[$EVENT_SWUPDATE_INVALID_CMD_ARGS]="Error"
EventType[$EVENT_SWUPDATE_MD5SUM_CHK_FAILED]="Error"
EventType[$EVENT_SWUPDATE_CRC_CHK_FAILED]="Error"
EventType[$EVENT_SWUPDATE_SLAVE_DOESNOT_HAVE_REQ_HW_VER]="Error"
EventType[$EVENT_SWUPDATE_NO_USB_DEV_FOUND]="Error"
EventType[$EVENT_SWUPDATE_UPDATE_PACKAGE_NOT_FOUND]="Error"
EventType[$EVENT_SWUPDATE_FILE_FOLDER_CREATION_FAILED]="Error"
EventType[$EVENT_SWUPDATE_SLAVE_UPDATE_FAILED]="Error"
EventType[$EVENT_SWUPDATE_FILE_FOLDER_DELETION_FAILED]="Error"
EventType[$EVENT_SWUPDATE_MISSING_SLAVE]="Error"
EventType[$EVENT_SWUPDATE_COPYING_FILE]="Info"
EventType[$EVENT_SWUPDATE_MISSING_XML_ATTR]="Error"
EventType[$EVENT_SWUPDATE_MOUNTING_DEV_PASSED]="Info"
EventType[$EVENT_SWUPDATE_UNZIP_HIGHEST_VER_UPDATE_PACKAGE]="Info"
EventType[$EVENT_SWUPDATE_CRC_CHK_PASSED]="Info"
EventType[$EVENT_SWUPDATE_MD5SUM_CHK_PASSED]="Info"
EventType[$EVENT_SWUPDATE_SLAVE_HAS_REQ_HW_VER]="Info"
EventType[$EVENT_SWUPDATE_FILE_FOLDER_CREATION]="Info"
EventType[$EVENT_SWUPDATE_EXTRACTING_PACKAGE]="Info"
EventType[$EVENT_SWUPDATE_EXECUTING_COMPONENT]="Info"
EventType[$EVENT_SWUPDATE_ROLLINGBACK]="Info"
EventType[$EVENT_SWUPDATE_PARSING_XML]="Info"
EventType[$EVENT_SWUPDATE_SUCCESS]="Info"
EventType[$EVENT_SWUPDATE_MOUNTING_DEV_FAILED]="Error"
EventType[$EVENT_SWUPDATE_SWVERSION_FILE_NOT_FOUND]="Info"
EventType[$EVENT_SWUPDATE_SWVERSION_FILE_CORRUPTED]="Info"
EventType[$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED]="Error"
EventType[$EVENT_SWUPDATE_STARTER_SUCCESS]="Info"
EventType[$EVENT_SWUPDATE_PTS_SUCCESS]="Info"
EventType[$EVENT_SWUPDATE_PTS_FAILED]="Info"
EventType[$EVENT_SWUPDATE_USB_UNMOUNTED]="Info"
EventType[$EVENT_SWUPDATE_BOOTCONFIG_FILE_NOT_FOUND]="Info"
EventType[$EVENT_SWUPDATE_BOOTCONFIG_FILE_UPDATE_SUCCESS]="Info"
EventType[$EVENT_SWUPDATE_BOOTCONFIG_UPDATE_ENTRY_MISSING]="Info"
EventType[$EVENT_SWUPDATE_UPDATING_FILE]="Info"
EventType[$EVENT_SWUPDATE_POWERFAILED]="Error"
EventType[$EVENT_SWUPDATE_ROLLBACK_SWVERSION]="Info"
EventType[$EVENT_SWUPDATE_FAILED]="Error"
EventType[$EVENT_SWUPDATE_REMOVE_OBSOLETE_FILE]="Info"
EventType[$EVENT_SWUPDATE_CONFIG_FILE_MODIFIED]="Info"
EventType[$EVENT_SWUPDATE_XML_NODE_ERROR]="Error"
EventType[$EVENT_SWUPDATE_ROLLBACKFAILED]="Error"
EventType[$EVENT_SWUPDATE_ERROR_MD5SUM]="Error"
EventType[$EVENT_SWUPDATE_CHECK_SUCCESS]="Info"


BASE_EVENT_ID=0				# Store the base event id with zero, this will be updated when Main sends the starting number
SWUPDATESCRIPT="MasterSWUpdate.sh"	# Store the script file name
USBMounted=false			# Store the mount value
UpdateStarted=false			# Store the SW update start value
RollbackFailed=false        # Flag indicating rollback failure

