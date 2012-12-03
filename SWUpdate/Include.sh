#!/bin/bash

#****************************************************************************/
 #! \file IncludeVariable.sh
 #
 #  \brief Script contains directory and file paths which are constant 
 #	   and functions like Log(), ExitOnError() to log the events.
 #	   It also contains a general function to compare 2 numbers.
 #
 #   $Version: $ 0.3
 #   $Date:    $ 2012-11-09
 #   $Author:  $ Vikas Jhunjhunwala
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


readonly ROOTDIR="/sd"					# Path of Root Directory 
if [ ! -d $ROOTDIR ]; then
	echo "$ROOTDIR does not exist"
	exit 1
fi
readonly BINDIR="$ROOTDIR/bin"				# Path of SW binaries
readonly FIRMWAREDIR="$ROOTDIR/Firmware"		# Path of Firmware binaries
readonly LOGDIR="$ROOTDIR/Log"				# Path of Log folder
readonly REMOTECAREDIR="$ROOTDIR/RemoteCare"		# Path of RemoteCare folder
readonly ROLLBACKDIR="$ROOTDIR/Rollback"		# Path of Rollback folder 
readonly SETTINGDIR="$ROOTDIR/Settings"			# Path of current configuration settings folder
readonly STARTDIR="$ROOTDIR/StartUp"			# Path of StartUp script
readonly TMPDIR="$ROOTDIR/tmp"				# Temporary folder
readonly UPDATEDIR="$TMPDIR/MasterSWUpdate"
readonly UPDATEPKGDIR="$UPDATEDIR/UpdatePackage"
readonly TMPSETTINGDIR="$UPDATEPKGDIR/Settings" 		# Temporary settings folder
readonly TMPBINDIR="$UPDATEPKGDIR/bin"				# Temporary SW binary folder
readonly TMPFIRMWAREDIR="$UPDATEPKGDIR/Firmware"		# Temporary Firmware binary
readonly SLAVEFILEDIR="$UPDATEDIR/SlaveSWUpdateFiles"
readonly USBDEVICEPATH="/dev/sda1"				# USB device file 
readonly MOUNTPOINT="/mnt" 					# USB Mount point
readonly REMOTECAREPATH="$ROOTDIR/RemoteCare"  			# RemoteCare Package path
readonly ROLLSWVERFILE="$ROLLBACKDIR/Settings/SW_Version.xml" 	# Path of SW Version file in Rollback folder 
readonly HWVERFILE="$SETTINGDIR/Slave_HW_Version.txt"		 # HW_Version file present on device
readonly UPDATEHWVERFILE="$TMPSETTINGDIR/Slave_HW_Version.txt" # HW_Version file in SW Package
readonly SWUPDATEFILE="$STARTDIR/SWUpdate.txt"			 # File to hold the flag to check for
								 # SW Update in progress or not
readonly SWVERFILE="$SETTINGDIR/SW_Version.xml"			 # SW_Version.xml file present on device
readonly TMPSWVERSIONFILE=$TMPDIR/temp_SW_Version.xml		 # Temporary file used for intermediate processing
readonly UPDATESWVERFILE="$TMPSETTINGDIR/SW_Version.xml"	 # SW_Version.xml file in SW package
readonly Md5FILE="$UPDATEPKGDIR/md5sum.txt"

if [ -f $HWVERFILE ]; then
	NAME=$(head -1  $HWVERFILE)
else
	echo "$HWVERFILE file missing"				# Since Log file does not exist at this point 
	exit 1							# so echoing error message to console	
fi

readonly DEVICENAME=${NAME%?}					 # Extracting Device name from Slave_HW_Version.txt
readonly DATE=`date +%Y-%m-%d,%T:%N | sed 's/......$//'`  	 # TimeStamp in format YYYY-MM-DD,HH:MM:SS:MS
readonly LOGFILE=$DEVICENAME"_SW_Update_Events.log"		 # Log file
readonly PKGFOOTER="_SW_Update_Package_"			 # Package name footer
SWUPDATESCRIPT="MasterSWUpdate.sh"


#=== FUNCTION ================================================================
# NAME: ExitOnError
# DESCRIPTION: This logs the error message to logfile and removes the temp files.
# PARAMETER 1: Error message 
#=============================================================================

ExitOnError()
{
        echo "$DATE, $$, Error:  $PROGNAME, SW Update failed; $1" >> $LOGDIR/$LOGFILE
        Clean
        exit 1
}

#=== FUNCTION ================================================================
# NAME: Log
# DESCRIPTION: Creates an entry in log file with a specific format.
# PARAMETER 1: Log message
#=============================================================================

Log()
{
        echo "$DATE, $$, Info: $PROGNAME, $1" >> $LOGDIR/$LOGFILE
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
	echo $1 | awk '{if($1 > '$2')\
        	               {print "GREATER"} \
                       else if($1 < '$2')\
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


