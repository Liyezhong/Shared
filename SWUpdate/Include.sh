#!/bin/bash

#****************************************************************************/
 #! \file Include.sh
 #
 #  \brief Script contains directory and file paths which are constant 
 #	   and functions like Log(), ExitOnError() to log the events.
 #	   It also contains a general function to compare 2 numbers.
 #
 #   $Version: $ 1.0, 2.0, 3.0
 #   $Date:    $ 2013-04-04, 2013-05-22, 2013-06-19
 #   $Author:  $ Hemanth Kumar
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


readonly ROOTDIR="/home/Leica"					# Path of Root Directory 
if [ ! -d $ROOTDIR ]; then
	echo "$ROOTDIR does not exist"
	exit 1
fi
readonly BINDIR="$ROOTDIR/Bin"				# Path of SW binaries
readonly FIRMWAREDIR="$ROOTDIR/Firmware"		# Path of Firmware binaries
readonly LOGDIR="$ROOTDIR/Logfiles"				# Path of Log folder
readonly ROLLBACKDIR="$ROOTDIR/Rollback"		# Path of Rollback folder 
readonly SETTINGDIR="$ROOTDIR/Settings"			# Path of current configuration settings folder
readonly STARTDIR="$ROOTDIR/StartUp"			# Path of StartUp script
readonly TMPDIR="$ROOTDIR/tmp"				# Temporary folder
readonly UPDATEDIR="$TMPDIR/MasterSWUpdate"		# Path to Master SW Update directory		
readonly UPDATEPKGDIR="$UPDATEDIR/UpdatePackage"		# Path to update package
readonly TMPSETTINGDIR="$UPDATEPKGDIR/Settings" 		# Temporary settings folder
readonly TMPBINDIR="$UPDATEPKGDIR/Bin"				# Temporary SW binary folder
readonly TMPFIRMWAREDIR="$UPDATEPKGDIR/Firmware"		# Temporary Firmware binary
readonly SLAVEFILEDIR="$UPDATEDIR/SlaveSWUpdateFiles"	# Path to slave SW update files
readonly MOUNTPOINT="/mnt" 					# USB Mount point
readonly REMOTECAREPATH="$ROOTDIR/RemoteCare"  			# RemoteCare Package path
readonly ROLLSWVERFILE="$ROLLBACKDIR/Settings/SW_Version.xml" 	# Path of SW Version file in Rollback folder 
readonly HWVERFILE="$SETTINGDIR/Slave_HW_Version.txt"		 # HW_Version file present on device
readonly UPDATEHWVERFILE="$TMPSETTINGDIR/Slave_HW_Version.txt" # HW_Version file in SW Package
readonly SWVERFILE="$SETTINGDIR/SW_Version.xml"			 # SW_Version.xml file present on device
readonly TMPSWVERSIONFILE=$TMPDIR/temp_SW_Version.xml		 # Temporary file used for intermediate processing
readonly UPDATESWVERFILE="$TMPSETTINGDIR/SW_Version.xml"	 # SW_Version.xml file in SW package
readonly MD5FILE="$UPDATEPKGDIR/md5sum.txt"		# md5 location in package directory
readonly TMPDIRMD5FILE="$TMPDIR/md5sum.txt"		# md5 file
readonly ROLLBACKBINDIR="$ROLLBACKDIR/Bin"		 	# Rollback bin directory
readonly ROLLBACKFIRMWAREDIR="$ROLLBACKDIR/Firmware"	# Rollback firmware directory
readonly PTSFILE="$TMPBINDIR/PTS"		 		# PTS file
readonly SLAVEUPDATEFILE="$TMPBINDIR/SlaveUpdate.js"	# Slave update java script file
readonly REBOOTFILE="$SETTINGDIR/Reboot.txt"		# Reboot.txt file present on device

readonly FINDBINDIR="Bin/"				# Path of bin directory folder - used for md5checksum
readonly FINDFIRMWAREDIR="Firmware/"				# Path of Firmware directory folder - used for md5checksum
readonly FINDSETTINGDIR="Settings/"				# Path of Settings directory folder - used for md5checksum


if [ -f $HWVERFILE ]; then
	NAME=$(head -1  $HWVERFILE)
else
	echo "$HWVERFILE file missing"				# Since Log file does not exist at this point 
	exit 1							# so echoing error message to console	
fi

readonly DEVICENAME=${NAME%?}				 # Extracting Device name from Slave_HW_Version.txt
readonly LOGFILE=$DEVICENAME"_SW_Update_Events.log"		 # Log file
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
readonly EVENT_SWUPDATE_REBOOT_FILE_NOT_FOUND=34
readonly EVENT_SWUPDATE_REBOOT_FILE_UPDATE_SUCCESS=35
readonly EVENT_SWUPDATE_REBOOT_UPDATE_ENTRY_MISSING=36


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
EventString[$EVENT_SWUPDATE_SLAVE_DOESNOT_HAVE_REQ_HW_VER]="SWUpdate Failed:Slave %s has HW version %s which is not equal or higher compared to %s"
EventString[$EVENT_SWUPDATE_NO_USB_DEV_FOUND]="SWUpdate Failed:No USB Device found"
EventString[$EVENT_SWUPDATE_UPDATE_PACKAGE_NOT_FOUND]="SWUpdate Failed:No Update Package found at %s"
EventString[$EVENT_SWUPDATE_FILE_FOLDER_CREATION_FAILED]="SWUpdate Failed:File/Folder creation failed for %s"
EventString[$EVENT_SWUPDATE_SLAVE_UPDATE_FAILED]="SWUpdate Failed: Slave SW update failed"
EventString[$EVENT_SWUPDATE_FILE_FOLDER_DELETION_FAILED]="SWUpdate Failed:File/Folder deletion failed for %s"
EventString[$EVENT_SWUPDATE_MISSING_SLAVE]="SWUpdate Failed:Missing slave name %s in %s"
EventString[$EVENT_SWUPDATE_COPYING_FILE]="SWUpdate:Copying the File/Folder from %s to %s"
EventString[$EVENT_SWUPDATE_MISSING_XML_ATTR]="SWUpdate:XML attribute %s found in %s but not in %s"
EventString[$EVENT_SWUPDATE_MOUNTING_DEV_PASSED]="SWUpdate:Mounting USB Device %s to %s passed"
EventString[$EVENT_SWUPDATE_UNZIP_HIGHEST_VER_UPDATE_PACKAGE]="SWUpdate: Unzipping the update package %s to %s"
EventString[$EVENT_SWUPDATE_CRC_CHK_PASSED]="SWUpdate:CRC check passed"
EventString[$EVENT_SWUPDATE_MD5SUM_CHK_PASSED]="SWUpdate:Md5sum check passed"
EventString[$EVENT_SWUPDATE_SLAVE_HAS_REQ_HW_VER]="SWUpdate:Slave %s has HW version %s is equal or higher compared to %s"
EventString[$EVENT_SWUPDATE_FILE_FOLDER_CREATION]="SWUpdate:Creating File/Folder %s"
EventString[$EVENT_SWUPDATE_EXTRACTING_PACKAGE]="SWUpdate:Extracting package from %s"
EventString[$EVENT_SWUPDATE_EXECUTING_COMPONENT]="SWUpdate:Executing the script %s"
EventString[$EVENT_SWUPDATE_ROLLINGBACK]="SWUpdate:Rolling back the files in %s from %s"
EventString[$EVENT_SWUPDATE_PARSING_XML]="SWUpdate:Parsing the XML file %s"
EventString[$EVENT_SWUPDATE_SUCCESS]="SWUpdate:Success"
EventString[$EVENT_SWUPDATE_MOUNTING_DEV_FAILED]="SWUpdate:Mounting USB Device %s to %s failed"
EventString[$EVENT_SWUPDATE_SWVERSION_FILE_NOT_FOUND]="SWUpdate:%s file not found. Updating all SW binaries and firmwares"
EventString[$EVENT_SWUPDATE_SWVERSION_FILE_CORRUPTED]="SWUpdate:%s file corrupted. Updating all SW binaries and firmwares"
EventString[$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED]="SWUpdate Failed:%s file corrupted."
EventString[$EVENT_SWUPDATE_STARTER_SUCCESS]="SWUpdate:Execution of the starter succeeded"
EventString[$EVENT_SWUPDATE_PTS_SUCCESS]="SWUpdate:PTS execution is success"
EventString[$EVENT_SWUPDATE_PTS_FAILED]="SWUpdate:PTS execution is failed"
EventString[$EVENT_SWUPDATE_USB_UNMOUNTED]="SWUpdate:Unmounted USB to %s"
EventString[$EVENT_SWUPDATE_REBOOT_FILE_NOT_FOUND]="SWUpdate:%s file not found"
EventString[$EVENT_SWUPDATE_REBOOT_FILE_UPDATE_SUCCESS]="SWUpdate:%s file updated"
EventString[$EVENT_SWUPDATE_REBOOT_UPDATE_ENTRY_MISSING]="SWUpdate:%s file missing entry of 'Software_Update_Status'"

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
EventLevel[$EVENT_SWUPDATE_REBOOT_FILE_NOT_FOUND]="Low"
EventLevel[$EVENT_SWUPDATE_REBOOT_FILE_UPDATE_SUCCESS]="Low"
EventLevel[$EVENT_SWUPDATE_REBOOT_UPDATE_ENTRY_MISSING]="Low"

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
EventType[$EVENT_SWUPDATE_REBOOT_FILE_NOT_FOUND]="Info"
EventType[$EVENT_SWUPDATE_REBOOT_FILE_UPDATE_SUCCESS]="Info"
EventType[$EVENT_SWUPDATE_REBOOT_UPDATE_ENTRY_MISSING]="Info"



BASE_EVENT_ID=0				# Store the base event id with zero, this will be updated when Main sends the starting number
SWUPDATESCRIPT="MasterSWUpdate.sh"	# Store the script file name
PTS_TIMEOUT="10m"				# Store the time out value for PTS
IsMounted=false			# Store the mount value
IsUpdateStarted=false			# Store the SW update start value


#=== FUNCTION ================================================================
# NAME: ExitOnError
# DESCRIPTION: Removes the temp file and logs the error message in log file and exits it
# PARAMETER 1: Software name (Either Master SW or Slave SW)
# PARAMETER 2: Event ID number
# PARAMETER 3: Argument that needs to be replaced in event string
# PARAMETER 4: Argument that needs to be replaced in event string
# PARAMETER 5: Argument that needs to be replaced in event string 
#=============================================================================

ExitOnError()
{	
	# IN1424 fix
	DATE=`date +%Y-%m-%d,%T:%N | sed 's/......$//'`
	# check whether USB is mounted, if mounted then unmount it
	if [ $IsMounted == true ]; then
		UnmountUSB
	fi

	# check whether SW update started
	if [ $IsUpdateStarted == true ]; then
		UpdateRebootFile "Failure"
	fi

	
	if [ $# -gt 1 ]; then        
		if [ $# -eq 2 ]; then
			printf "$DATE, $1, $(($2+BASE_EVENT_ID)), ${EventType[$2]}, ${EventLevel[$2]}, ${EventString[$2]}\n" >> $LOGDIR/$LOGFILE
		elif [ $# -eq 3 ]; then
			printf "$DATE, $1, $(($2+BASE_EVENT_ID)), ${EventType[$2]}, ${EventLevel[$2]}, ${EventString[$2]}\n" "$3" >> $LOGDIR/$LOGFILE
		elif [ $# -eq 4 ]; then
			printf "$DATE, $1, $(($2+BASE_EVENT_ID)), ${EventType[$2]}, ${EventLevel[$2]}, ${EventString[$2]}\n" "$3" "$4" >> $LOGDIR/$LOGFILE
		else
			printf "$DATE, $1, $(($2+BASE_EVENT_ID)), ${EventType[$2]}, ${EventLevel[$2]}, ${EventString[$2]}\n" "$3" "$4" "$5" >> $LOGDIR/$LOGFILE
	
		fi
		# add an empty line for readbility in log file
		printf "\n" >> $LOGDIR/$LOGFILE
	fi
	
        Clean
        exit 1
}

#=== FUNCTION ================================================================
# NAME: Logs the error message in log file
# DESCRIPTION: Creates an entry in log file with a specific format.
# PARAMETER 1: Software name (Either Master SW or Slave SW)
# PARAMETER 2: Event ID number
# PARAMETER 3: Argument that needs to be replaced in event string
# PARAMETER 4: Argument that needs to be replaced in event string
# PARAMETER 5: Argument that needs to be replaced in event string
#=============================================================================

Log()
{
	# IN1424 fix	
	DATE=`date +%Y-%m-%d,%T:%N | sed 's/......$//'`	
	# check for the Parameter count, if the parameter count greater than zero then log the data	
	if [ $# -gt 1 ]; then        
		if [ $# -eq 2 ]; then						
			printf "$DATE, $1, $(($2+BASE_EVENT_ID)), ${EventType[$2]}, ${EventLevel[$2]}, ${EventString[$2]}\n" >> $LOGDIR/$LOGFILE
		elif [ $# -eq 3 ]; then
			printf "$DATE, $1, $(($2+BASE_EVENT_ID)), ${EventType[$2]}, ${EventLevel[$2]}, ${EventString[$2]}\n" "$3" >> $LOGDIR/$LOGFILE
		elif [ $# -eq 4 ]; then
			printf "$DATE, $1, $(($2+BASE_EVENT_ID)), ${EventType[$2]}, ${EventLevel[$2]}, ${EventString[$2]}\n" "$3" "$4" >> $LOGDIR/$LOGFILE
		else
			printf "$DATE, $1, $(($2+BASE_EVENT_ID)), ${EventType[$2]}, ${EventLevel[$2]}, ${EventString[$2]}\n" "$3" "$4" "$5" >> $LOGDIR/$LOGFILE
	
		fi
		# add an empty line for readbility in log file
		printf "\n" >> $LOGDIR/$LOGFILE
	fi
}

#=== FUNCTION ================================================================
# NAME: CompareVersion
# DESCRIPTION: 	This Compares two floating point version numbers 
#		e.g. CompareVersion 1.234 2.345
#		Return GREATER if FirstNumber > SecondNumber
#		Return LOWER if FirstNumber < SecondNumber
#		Return EQUAL if FirstNumber = SecondNumber
# PARAMETER 1: 	FirstNumber
# PARAMETER 2:	SecondNumber
#=============================================================================

CompareVersion()
{	
	awk 'BEGIN {if('$1' > '$2')\
        	{print "GREATER"} \
             else if('$1' < '$2')\
                 {print "LOWER"}\
             else\
                 {print "EQUAL"}}'
}

#=== FUNCTION ================================================================
# NAME: Clean
# DESCRIPTION: Clears data from temp folder
# PARAMETER 1: NA
#=============================================================================

Clean()
{
	rm -rf $TMPDIR
}

#=== FUNCTION ================================================================
# NAME: Unmount
# DESCRIPTION: Unmounts the mounted USB device
# PARAMETER 1: NA
#=============================================================================

UnmountUSB()
{	
	
	umount -l $MOUNTPOINT &>/dev/null
	# IN1296 fix
	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_USB_UNMOUNTED" "$MOUNTPOINT"
	IsMounted=false
	
}

#=== FUNCTION ================================================================
# NAME: UpdateRebootFile
# DESCRIPTION: Updates reboot file with failure or success
# PARAMETER 1: String value (Failure or Success)
#=============================================================================

UpdateRebootFile()
{	
	# check whether Reboot.txt file exists or not
	if [ ! -f $REBOOTFILE ]; then
		Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_REBOOT_FILE_NOT_FOUND" "$REBOOTFILE"
	else		
		# Check whether SW update text is available, If yes replace the text
		grep -q "Software_Update_Status" "$REBOOTFILE"	
		if [ $? -eq 1 ];then
			# Add entry otherwise
			echo "Software_Update_Status:$1" >> $REBOOTFILE
			# Log the event in log file
			Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_REBOOT_UPDATE_ENTRY_MISSING" "$REBOOTFILE"
		else		
			# replace the text whether status is pass or failure
			sed -i '/Software_Update_Status/c\Software_Update_Status:'"$1" $REBOOTFILE
		fi
		# Log the event in log file
		Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_REBOOT_FILE_UPDATE_SUCCESS" "$REBOOTFILE"
	fi
	IsUpdateStarted=false	
	
}
