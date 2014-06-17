#****************************************************************************/
 #! \file Util.sh
 #
 #  \brief  Contains general functions required for SW Update
 #
 #   $Version: $ 1.0
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
# Extract directory path from where this script is executing
CURRDIR="$( cd "$( dirname "$0" )" && pwd )"

if [ -f $CURRDIR/Include.sh ]; then
	. $CURRDIR/Include.sh
else
	echo "$CURRDIR/Include.sh file missing"
	exit 1
fi		


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
	DATE=`date +%Y-%m-%d,%T:%N | sed 's/......$//'`
	# check whether USB is mounted, if mounted then unmount it
	[ "$USBMounted" = "true" ] && UnmountUSB

	# check whether SW update started
	if [ "$UpdateStarted" = "true" ]; then
        if [ "$RollbackFailed" = "true" ];then
            #Rollback has failed, InitScript shall read this status and display 
            #information to user regarding corrupt software.
            UpdateBootConfigFile "Software_Update_Status" "RollbackFailed"
        else
            UpdateBootConfigFile "Software_Update_Status" "Failure"
        fi
        kill -9 $(pidof ImageTestApp) > /dev/null 2>&1
        kill -9 $(pidof -x PowerFailMonitor.sh) > /dev/null 2>&1
    fi
    
    Clean
    Log "$@"
    Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FAILED"
    exit 1
}


#=== FUNCTION ================================================================
# NAME: ExitAndReboot
# DESCRIPTION: Exits SWUpdate and Reboots OS
# PARAMETER 1: NA
#=============================================================================
ExitAndReboot()
{
	# check whether USB is mounted, if mounted then unmount it
	[ "$USBMounted" = "true" ] && UnmountUSB
	# check whether SW update started
	[ "$UpdateStarted" == "true" ] && UpdateBootConfigFile  "Software_Update_Status" "Failure"
    Clean
    Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FAILED"
    #reboots OS
    reboot &
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
    /home/Leica/Scripts/EBox-Utils.sh  umount_usb > /dev/null 2>&1	
	# IN1296 fix
	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_USB_UNMOUNTED" "$MOUNTPOINT"
	USBMounted=false
}

#=== FUNCTION ================================================================
# NAME: UpdateBootConfigFile
# DESCRIPTION: Updates reboot file with failure or success
# PARAMETER 1: BootConfig file parameter
# PARAMETER 2: BootConfig file parameter value
#              
#=============================================================================
UpdateBootConfigFile()
{	
    # Check whether "PowerFailed" text is available, If yes replace the text
    grep -q $1 "$BOOTCONFIGFILE"
    if [ $? -eq 1 ];then #not found
            # Add entry otherwise
            echo "$1:$2" >> $BOOTCONFIGFILE
            # Log the event in log file
            Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_BOOTCONFIG_UPDATE_ENTRY_MISSING" "$BOOTCONFIGFILE"
    else
            # replace the text whether status is pass or failure
            sed -i "/$1/c\\$1:$2" $BOOTCONFIGFILE
    fi
	UpdateStarted=false
    cd $SETTINGDIR
    readonly local BootConfigFileName=$(basename $BOOTCONFIGFILE)
    readonly local BootConfigFileMd5sum=$(md5sum $BootConfigFileName)
    sed -i "/\s$BootConfigFileName\b/c\\$BootConfigFileMd5sum" $SETTINGDIR/.md5sum.txt
	cd - > /dev/null 2>&1
}

#=== FUNCTION ================================================================
# NAME: PowerFailed
# DESCRIPTION: Logs power failure
#=============================================================================
PowerFailed()
{
    Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_POWERFAILED"
}


#=== FUNCTION ================================================================
# NAME: ASB_Filter
# PARAMETER1: ASB binary name - e.g.:ASB1_0.bin
# DESCRIPTION: Function to filter ASB's names from update list if that particular
#       ASB is not available on the instrument
#=============================================================================
ASB_Filter()
{
    local FIRMWARES_TO_BE_UPDATED_FILTERED_LOCAL=()
    #create a temporary updateable firmware list
    FIRMWARES_TO_BE_UPDATED_FILTERED_LOCAL=(${FIRMWARES_TO_BE_UPDATED[@]%%$1})
    #clear global array, we will update it with filtered list
    unset FIRMWARES_TO_BE_UPDATED
    #clone back to global array
    FIRMWARES_TO_BE_UPDATED=(${FIRMWARES_TO_BE_UPDATED_FILTERED_LOCAL[@]})
}

