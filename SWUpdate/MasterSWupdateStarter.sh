#!/bin/bash

#****************************************************************************/
 #! \file MasterSWupdateStarter.sh
 #
 #  \brief: This module verifies the CRC, MD5Sum of the SW Update package and
 #	    also verifies the Slave HW version. This also invokes the 
 #	    MasterSWUpdate.sh to upgrade the SW binaries and SlaveFW 
 #
 #   $Version: $ 0.4
 #   $Date:    $ 2012-11-16
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

# Extract directory path from where this script is executing
CURRDIR="$( cd "$( dirname "$0" )" && pwd )"

if [ -f $CURRDIR/Include.sh ];then
	. $CURRDIR/./Include.sh
else
	echo "$CURRDIR/Include.sh file missing"
fi		

# Extract current script name
PROGNAME=$(basename $0)	

#Command line argument		
CMDARG="Usage: $0 filename [-check USB/RemoteCare] [-update/updateRollback]"

# Clean Temp file on Abort or terminate
trap Clean ABRT TERM 		

#=== FUNCTION ======================================================================
# NAME: MountUSB
# DESCRIPTION: Mount USB device 
# PARAMETER: NA
#===================================================================================

MountUSB()
{
	ls /dev/sd* &>/dev/null
	if [ $? -ne 0 ];then
		ExitOnError "No USB device present"
	fi
	Log "Mounting USB Device"
	TotalMountPt=$(ls /dev/sd* | wc -l)
	Counter=0
	for device in $(ls /dev/sd*)
	do
        	((Counter++))
                mount $device $PKGPATH &>/dev/null
		local Ret=$?
		if [ $Ret -ne 0 ]; then
                	Log "Mounting $device on $PKGPATH failed"
                	continue
		elif [ $Ret -eq 0 ]; then
                	Log "Mounted USB device $device on $PKGPATH"
			CountPackage
			if [ $PkgFound -eq 1 ];then
				break
			elif [ $PkgFound -ne 1 ]; then
                		umount $PKGPATH &>/dev/null
				Log "SW Package not present at $device"
			fi
		fi
	
		if [ $Counter -eq $TotalMountPt ]; then
			ExitOnError "Scanned all the USB devices"
		fi
	done
}


#=== FUNCTION =========================================================================
# NAME: CountPackage
# DESCRIPTION:  Read the number of SW Packages present in USB 
# PARAMETER: NA
#======================================================================================

CountPackage()
{
	ls $PKGPATH/"$DEVICENAME""$PKGFOOTER"*.zip &>/dev/null
	local Ret=$?
	if [ $Ret -eq 0 ] ; then
                PkgCount=$(ls $PKGPATH/"$DEVICENAME""$PKGFOOTER"*.zip | wc -l)
		Log "$PkgCount SW package present at $PKGPATH"
		PkgFound=1
	else
		PkgFound=0
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

        Log "$PkgVersion is highest version of update package present at $PKGPATH"

        PkgName="$DEVICENAME""$PKGFOOTER""$PkgVersion"_"$PkgDate"
        
        mkdir -p $UPDATEDIR || ExitOnError "Creating $UPDATEDIR folder"
        cp $PKGPATH/$PkgName $UPDATEDIR/
	if [ $? -ne 0 ]; then
		ExitOnError "Copying $PKGPATH/$PkgName to $UPDATEDIR folder"
	fi
	
	Log "Checking for CRC"

	# Check for CRC of zipped SW package
        unzip -t $UPDATEDIR/$PkgName &>/dev/null
        if [ $? -eq 0 ]; then
        	Log "CRC check passed" && Log "Unzipping $PkgName package"
		
		# Unzip SW Update Package
           	unzip -d $UPDATEPKGDIR $UPDATEDIR/$PkgName &>/dev/null
		[ -d $UPDATEPKGDIR/bin ] && [ -d $UPDATEPKGDIR/Firmware ] && [ -d $UPDATEPKGDIR/Settings ]\
                && [ -f $UPDATESWVERFILE ] || ExitOnError "Could not find\
                 $UPDATEPKGDIR/bin, $UPDATEPKGDIR/Firmware, $UPDATEPKGDIR/Settings and $UPDATESWVERFILE\
                 at $UPDATEPKGDIR"
	else
                ExitOnError "CRC check"
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
		ExitOnError "$Md5FILE file not present"
	fi

	find bin/ Settings/ Firmware/ -type f -print\
        | xargs md5sum >> $TMPDIR/md5sum.txt
	

	Log "Performing md5sum check"
        md5sum -c $Md5FILE &>/dev/null
	if [ $? -eq 0 ]
        then
	       	Log "md5sum check: No file missing/corrupted"
	else
               	ExitOnError "md5sum check: file missing/corrupted"
        fi
	
	LineCountPkgMd5=$(cat $Md5FILE | wc -l)
	LineCountGeneratedMd5=$(cat $TMPDIR/md5sum.txt | wc -l)

	if [ $LineCountPkgMd5 -eq $LineCountGeneratedMd5 ]
        then
	       	Log "md5sum check: No unknown file present in SW package"
		Log "md5sum check: Passed"
	else
               	ExitOnError "md5sum check: Unknown file present in SW package"
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
		ExitOnError "$UPDATEHWVERFILE not present"
	fi

	if [ ! -f $HWVERFILE ]; then  
		ExitOnError "$HWVERFILE not present"
	fi

	Log "Checking for minimum Slave HW Version required to upgrade"
	
	SlaveFile="tail -n+2 $HWVERFILE"
	for CurrentSlaveName in $( ${SlaveFile} | cut -d',' -f1 )
	do
		CurrHW="grep $CurrentSlaveName $HWVERFILE"
		ReqHW="grep $CurrentSlaveName $UPDATEHWVERFILE"
		
		grep $CurrentSlaveName $HWVERFILE &>/dev/null
		if [ $? -ne 0 ]; then
			ExitOnError "$CurrentSlaveName not present in $HWVERFILE"
		fi
		
		grep $CurrentSlaveName $UPDATEHWVERFILE &>/dev/null
		if [ $? -ne 0 ]; then
			ExitOnError "$CurrentSlaveName not present in $UPDATEHWVERFILE"
		fi
		
		CurrentHWVersion=$( ${CurrHW} | cut -d',' -f2)
                RequiredHWVersion=$( ${ReqHW} |cut -d',' -f2)
				
		Return=$(CompareVersion $CurrentHWVersion $RequiredHWVersion)
								                
		if [ $Return == "LOWER" ]; then
                	ExitOnError "$CurrentSlaveName does not have required minimum version $RequiredHWVersion" 
		elif [ $Return == "EQUAL" ]; then
			Log "$CurrentSlaveName has equal version" 
		else
			Log "$CurrentSlaveName has higher version" 
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
	Log "Device is $DEVICENAME" 
fi


#==============================================================
# Script Begins from Here
# -check:   Check for the SW package in USB or Remote care
# -update:  Execute the MasterSWUpdate script to update the SW
#	    and Slave FW Files
# -updateRollback:Execute the MasterSWUpdate script to update the 
#		  Rollback folder. 
#==============================================================


[ $# -gt 2 ] || [ $# -lt 1 ] && { Log "$CMDARG"; exit 1; }

if [ $1 = "-check" ]; then
        if [ $2 = "USB" ]; then
		Clean
		# Path where package is available
                PKGPATH=$MOUNTPOINT	
		MountUSB
                UnzipAndCRC
		umount $PKGPATH
        elif [ $2 = "RemoteCare" ]; then
		Clean
		PKGPATH=$REMOTECAREPATH
		Log "Extracting package from RemoteCare folder"
		CountPackage
		UnzipAndCRC
	else
		Log "$CMDARG"
		exit 1
	fi

	CheckMd5Sum
	CheckSlaveHWVersion
	Log "Check Status: Passed"

elif [ $1 = "-update" ]; then
	if [ -f $TMPBINDIR/$SWUPDATESCRIPT ]; then
		exec $TMPBINDIR/./$SWUPDATESCRIPT $1
	else
		ExitOnError "$TMPBINDIR/$SWUPDATESCRIPT not present"
	fi

elif [ $1 = "-updateRollback" ]; then
	if [ -f $TMPBINDIR/$SWUPDATESCRIPT ]; then
		exec $TMPBINDIR/./$SWUPDATESCRIPT $1
	else
		ExitOnError "$TMPBINDIR/$SWUPDATESCRIPT not present"
	fi

	Clean
else
	Log "$CMDARG"
	exit 1
fi
