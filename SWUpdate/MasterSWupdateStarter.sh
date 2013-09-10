#!/bin/bash

#****************************************************************************/
 #! \file MasterSWupdateStarter.sh
 #
 #  \brief: This module verifies the CRC, MD5Sum of the SW Update package and
 #	    also verifies the Slave HW version. This also invokes the 
 #	    MasterSWUpdate.sh to upgrade the SW binaries and SlaveFW 
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

# Extract directory path from where this script is executing
CURRDIR="$( cd "$( dirname "$0" )" && pwd )"

if [ -f $CURRDIR/Include.sh ]; then
	. $CURRDIR/Include.sh
else
	echo "$CURRDIR/Include.sh file missing"
	exit 1
fi		

# Extract current script name
PROGNAME=$(basename $0)	

# Command line argument		
readonly CMDARG="Usage: $0 [-check USB/RemoteCare] [-update] [-updateRollback] [baseeventid]"

# Clean Temp file on Abort or terminate
#trap Clean ABRT TERM 		

#=== FUNCTION ======================================================================
# NAME: MountUSB
# DESCRIPTION: Mount USB device 
# PARAMETER: NA
#===================================================================================

MountUSB()
{
	blkid -t TYPE=vfat | grep -v "Max Touch" | grep -v "mmcblk" &>/dev/null
	if [ $? -ne 0 ]; then
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_NO_USB_DEV_FOUND"
	fi
	TotalMountPt=$(blkid -t TYPE=vfat | grep -v "Max Touch" | grep -v "mmcblk" | cut -d ":" -f1 | wc -l)
	Counter=0		
	for device in $(blkid -t TYPE=vfat | grep -v "Max Touch" | grep -v "mmcblk" | cut -d':' -f1);do
        	((Counter++))
		# Unmount the devices if already mounted
		# IN1426 fix
		Mounted=$(mount | grep "$device" | cut -d' ' -f1 | wc -l)
		if [ $Mounted -ne 0 ]; then
			# Recurssively unmount all the devices
			for mountedpath in $(mount | grep "$device" | cut -d' ' -f3);do
				
				umount -l $mountedpath &>/dev/null
				# IN1296 fix
				Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_USB_UNMOUNTED" "$mountedpath"
				
			done
		fi

                mount $device $PKGPATH &>/dev/null
		local Ret=$?
		if [ $Ret -ne 0 ]; then
                	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_MOUNTING_DEV_FAILED" "$PKGPATH"			
                	continue
		elif [ $Ret -eq 0 ]; then
                	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_MOUNTING_DEV_PASSED" "$device" "$PKGPATH"
			# IN1300 fix
			RetVal=$(CountPackage)
			if [ $RetVal == "1" ]; then				
				IsMounted=true				
				break
			elif [ $RetVal == "0" ]; then								
                		umount -l $PKGPATH &>/dev/null
				# IN1296 fix
				Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_USB_UNMOUNTED" "$PKGPATH"
				# If all the devices are enumerated then exit from the script because thers is no point to
				# go further
				if [ $TotalMountPt -eq $Counter ]; then
					ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_UPDATE_PACKAGE_NOT_FOUND" "$device"
				else
					Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_UPDATE_PACKAGE_NOT_FOUND" "$device"
				fi
			fi
		fi
		
	done
}


#=== FUNCTION =========================================================================
# NAME: CountPackage
# DESCRIPTION:  Read the number of SW Packages present in USB or in remote care
# PARAMETER: NA
#======================================================================================

CountPackage()
{
	ls $PKGPATH/"$DEVICENAME""$PKGFOOTER"*.zip &>/dev/null
	local Ret=$?
	if [ $Ret -eq 0 ]; then
                PkgCount=$(ls $PKGPATH/"$DEVICENAME""$PKGFOOTER"*.zip | wc -l)		
		echo "1"
	else
		echo "0"
	fi
}


#=== FUNCTION =========================================================================
# NAME: UnzipAndCRC
# DESCRIPTION: 	Checks for the higher version of SW package from the packages available.
#		Unzip higher version SW package and checks for CRC of package.
# PARAMETER: NA
#======================================================================================

UnzipAndCRC()
{
	Pkglist="ls $PKGPATH/"$DEVICENAME""$PKGFOOTER"*.zip"
        PkgVersion=$(${Pkglist} | cut -d'_' -f5 | sort | tail -1)

	Pkg="ls $PKGPATH/"$DEVICENAME""$PKGFOOTER""$PkgVersion"*.zip"
	PkgDate=$(${Pkg} | cut -d'_' -f6 | sort | tail -1)
	

        PkgName="$DEVICENAME""$PKGFOOTER""$PkgVersion"_"$PkgDate"
        
        mkdir -p $UPDATEDIR || ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_CREATION_FAILED" "$UPDATEDIR"

	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$PKGPATH/$PkgName" "$UPDATEDIR"

        cp $PKGPATH/$PkgName $UPDATEDIR/	

	if [ $? -ne 0 ]; then
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$PKGPATH/$PkgName" "$UPDATEDIR"
	fi	
	
	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_UNZIP_HIGHEST_VER_UPDATE_PACKAGE" "$PkgName" "$UPDATEDIR"

	# Check for CRC of zipped SW package
        unzip -t $UPDATEDIR/$PkgName &>/dev/null
        if [ $? -eq 0 ]; then
        	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_CRC_CHK_PASSED"
		
		# Unzip SW Update Package
           	unzip -d $UPDATEPKGDIR $UPDATEDIR/$PkgName &>/dev/null
		[ -d $TMPBINDIR ] && [ -d $TMPFIRMWAREDIR ] && [ -d $TMPSETTINGDIR ]\
                && [ -f $UPDATESWVERFILE ] || ExitOnError "$EVENT_SOURCE_MASTER"\
                "$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS"\
                "$TMPBINDIR, $TMPFIRMWAREDIR, $TMPSETTINGDIR and $UPDATESWVERFILE"
	else
                ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_CRC_CHK_FAILED"
        fi
}

#=== FUNCTION =========================================================================
# NAME: CheckMd5Sum
# DESCRIPTION:  Checking for Md5Sum of the SW Package
# PARAMETER: NA
#======================================================================================

CheckMd5Sum()
{
	cd $UPDATEPKGDIR

	if [ ! -f $Md5FILE ]; then
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS" "$MD5FILE"
	fi

#	find $FINDBINDIR $FINDSETTINGDIR $FINDFIRMWAREDIR -type f -print\
#       | xargs md5sum >> $TMPDIRMD5FILE
	find ./ -type f |grep -v './md5sum.txt'\
        | xargs md5sum >> $TMPDIRMD5FILE


        md5sum -c $MD5FILE &>/dev/null
	if [ $? -ne 0 ]; then	       	
               	ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_MD5SUM_CHK_FAILED"
        fi
	
	LineCountPkgMd5=$(cat $MD5FILE | wc -l)
	LineCountGeneratedMd5=$(cat $TMPDIRMD5FILE | wc -l)

	if [ $LineCountPkgMd5 -eq $LineCountGeneratedMd5 ]; then
		Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_MD5SUM_CHK_PASSED"
	else
               	ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_MD5SUM_CHK_FAILED"
        fi
	
	cd - &>/dev/null        
}

#=== FUNCTION =========================================================================
# NAME: CheckSlaveHWVersion
# DESCRIPTION: Checks for current slave HWs with minimum required slave HW Version 
# PARAMETER: NA
#======================================================================================

CheckSlaveHWVersion()
{
	if [ ! -f $UPDATEHWVERFILE ]; then  
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS" "$UPDATEHWVERFILE"		
	fi

	if [ ! -f $HWVERFILE ]; then  
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS" "$HWVERFILE"		
	fi

	UPDATEHWVERFILELINE=$(wc -l $UPDATEHWVERFILE | cut -d' ' -f1)
	HWVERFILELINE=$(wc -l $HWVERFILE | cut -d' ' -f1)
	if [ $UPDATEHWVERFILELINE -ne $HWVERFILELINE ]; then
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_MISSING_SLAVE" "$HWVERFILE"
	fi

	SlaveFile="tail -n+2 $HWVERFILE"

	for CurrentSlaveName in $( ${SlaveFile} | cut -d',' -f1 );do
		# When it searches the "Slave1" name in upgraded package then there is a chance that "Slave10" also takes 
		# into search criteria and the same will be logged in log file
		# IN1299 fix		
		CurrentSlaveName+=","

		CurrHW="grep $CurrentSlaveName $HWVERFILE"
		ReqHW="grep $CurrentSlaveName $UPDATEHWVERFILE"
		
		grep $CurrentSlaveName $UPDATEHWVERFILE &>/dev/null
		if [ $? -ne 0 ]; then
			ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_MISSING_SLAVE" "$CurrentSlaveName" "$UPDATEHWVERFILE"
		fi
		
		CurrentHWVersion=$( ${CurrHW} | cut -d',' -f2)
                RequiredHWVersion=$( ${ReqHW} |cut -d',' -f2)
		
		Return=$(CompareVersion $CurrentHWVersion $RequiredHWVersion)
								                
		if [ $Return == "LOWER" ]; then
			ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_SLAVE_DOESNOT_HAVE_REQ_HW_VER" "$CurrentSlaveName" "$CurrentHWVersion" "$RequiredHWVersion"		
		else
			# IN 1296 fix			
			Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_SLAVE_HAS_REQ_HW_VER" "$CurrentSlaveName" "$CurrentHWVersion" "$RequiredHWVersion"
		fi
	done
}

#--------------------------------------------------------------------
#	Making Log folder and Log file if it doesn't exist
#       Creating header entry in LOG File if log file is empty
#--------------------------------------------------------------------

mkdir -p $LOGDIR  2> /dev/null
if [ $? -ne 0 ]; then
	echo "Creating $LOGDIR failed"	
	exit 1
fi

if [ ! -s $LOGDIR/$LOGFILE ]; then
	echo -n "" >> $LOGDIR/$LOGFILE
	echo -e "Format Version: 1 \nComponent: Master SW Update \nTimeStamp: $DATE \n" >> $LOGDIR/$LOGFILE	
fi


#==============================================================
# Script Begins from Here
# -check:   Check for the SW package in USB or Remote care
# -update:  Execute the MasterSWUpdate script to update the SW
#	    and Slave FW Files
# -updateRollback:Execute the MasterSWUpdate script to update the 
#		  Rollback folder. 
#==============================================================


[ $# -gt 3 ] || [ $# -lt 2 ] && { ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_INVALID_CMD_ARGS" "$CMDARG"; }
# Third argument will be  event ID
FirstArgument=$1
SecondArgument=$2
if [ $FirstArgument = "-check" ]; then	        
	# IN1297 fix
	# Check whether at least three arguments are available or not	
	if [ $# -lt 3 ]; then
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_INVALID_CMD_ARGS" "$CMDARG"
	fi	
	BASE_EVENT_ID=$3	
	if [ $SecondArgument = "USB" ]; then
		Clean
		# Path where package is available
                PKGPATH=$MOUNTPOINT	
		MountUSB
                UnzipAndCRC
		UnmountUSB		
        elif [ $SecondArgument = "RemoteCare" ]; then
		Clean
		PKGPATH=$REMOTECAREPATH
		RetVal=$(CountPackage)
		if [ $RetVal == "1" ];then
			UnzipAndCRC
		elif [ $RetVal == "0" ]; then                	
			ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_UPDATE_PACKAGE_NOT_FOUND" "$PKGPATH"			
		fi		
	else
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_INVALID_CMD_ARGS" "$CMDARG"		
	fi

	CheckMd5Sum
	CheckSlaveHWVersion
elif [ $FirstArgument = "-update" ]; then
	# IN1297 fix
	# Check whether two arguments are available or not	
	if [ ! $# -eq 2 ]; then
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_INVALID_CMD_ARGS" "$CMDARG"
	fi
	BASE_EVENT_ID=$2
	if [ -f $TMPBINDIR/$SWUPDATESCRIPT ]; then
		Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_STARTER_SUCCESS"
		Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_EXECUTING_COMPONENT" "$TMPBINDIR/$SWUPDATESCRIPT"
		# IN1356 fix
		# Change script file permissions		
		chmod 755 $TMPBINDIR/$SWUPDATESCRIPT
		exec $TMPBINDIR/$SWUPDATESCRIPT $FirstArgument $BASE_EVENT_ID 
	else
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS" "$TMPBINDIR/$SWUPDATESCRIPT"
	fi

elif [ $FirstArgument = "-updateRollback" ]; then
	# IN1297 fix
	# Check whether at least two arguments are available or not	
	if [ ! $# -eq 2 ]; then
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_INVALID_CMD_ARGS" "$CMDARG"
	fi	
	BASE_EVENT_ID=$2
	if [ -f $TMPBINDIR/$SWUPDATESCRIPT ]; then
		Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_STARTER_SUCCESS"
		Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_EXECUTING_COMPONENT" "$TMPBINDIR/$SWUPDATESCRIPT"
		# IN1356 fix
		# Change script file permissions
		chmod 755 $TMPBINDIR/$SWUPDATESCRIPT
		exec $TMPBINDIR/$SWUPDATESCRIPT $FirstArgument $BASE_EVENT_ID 
	else
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS" "$TMPBINDIR/$SWUPDATESCRIPT"
	fi

	Clean
else	
	ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_INVALID_CMD_ARGS" "$CMDARG"
	
fi
