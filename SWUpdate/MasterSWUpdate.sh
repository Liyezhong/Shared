#!/bin/bash

#/****************************************************************************/
#/*! \file MasterSWUpdate.sh
 #
 #  \brief: 	This updates the new Software binaries and Slave Firmware also
 #		rollback the binaries and firmware on error.
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
#/****************************************************************************/

# Extract directory path from where this script is executing
CURRDIR="$( cd "$( dirname "$0" )" && pwd )"

if [ -f $CURRDIR/Include.sh ];then
	. $CURRDIR/./Include.sh
else
	Log "$CURRDIR/Include.sh file missing"
fi

# Extract current script name
PROGNAME=$(basename $0) 

CMDARG="Usage: $0 filename [-update/updateRollback]"

# Clean Temp file on Abort or terminate
trap Clean ABRT TERM  

# Rollback on keyboard interrupt
trap Rollback INT

#=== FUNCTION ================================================================
# NAME: MasterSWUpdate
# DESCRIPTION:  Check the presence of Rollback and SW_Version.xml file and 
#		update the latest SW binaries and Slave FW files.
#		If SW_Version.xml is not present on the device, update all the
#		the SW and FW files.	
# PARAMETER : NA 
#=============================================================================

MasterSWUpdate()
{
	# Checking for presence of Rollback folder
	if [ -d "$ROLLBACKDIR" ]; then
		Log "$ROLLBACKDIR folder present"

                # Checking for presence of SW_Version.xml on the device
                if [ -f $SWVERFILE ]; then
                        Log "$SWVERFILE file present" 
			UpdateSWBinaries
			UpdateSlaveFW
		else
                        Log "$SWVERFILE file not present"
			Log "Updating all SW files"
			
			if [ ! -d $TMPBINDIR ]; then
				ExitOnError "$TMPBINDIR not present"
			fi

			# Updating all SW binaries
			cp -r $TMPBINDIR/ $ROOTDIR/
			if [ $? -ne 0 ]; then
				RollbackSW
				ExitOnError "Copying binaries from $TMPBINDIR/ to $ROOTDIR/" 
			fi

			[ -f $BINDIR/$PROGNAME ] && rm $BINDIR/$PROGNAME			

			# Creating temp directory and copying all FW files for update
			mkdir -p $SLAVEFILEDIR 
			if [ $? -ne 0 ]; then
				RollbackFWAndSW
				ExitOnError "Error in creating $SLAVEFILEDIR directory" 
			fi
                        
			cp -r $TMPFIRMWAREDIR/* $SLAVEFILEDIR 
			if [ $? -ne 0 ]; then
				ExitOnError "Copying firmware files from $TMPFIRMWAREDIR to $SLAVEFILEDIR" 
			fi

			cp $UPDATESWVERFILE $SETTINGDIR/
			if [ $? -ne 0 ]; then
				RollbackFWAndSW
				ExitOnError "Copying $UPDATESWVERFILE to $SETTINGDIR" 
			fi	
		        
			FWUpdateStatus=$(ExecuteSlaveFWUpdate)
			if [ "$FWUpdateStatus" == "True" ]; then
				Log "Slave FW updated"
			else
				RollbackFWAndSW
				Log "while performing Rollback"
				ExitOnError "Slave FW update"
			fi

		fi
        else
        	ExitOnError "$ROLLBACKDIR folder not present"
        fi
}


#=== FUNCTION ===================================================================
# NAME: UpdateSlaveFW
# DESCRIPTION: 	Check for the higher version of firmware available at package
#		Copy the higher version firmware files to temp directory. 
#		Execute Slave FW Update SW to update required Slave device
#		Also updates the SW_Version.xml file 
# PARAMETER : NA 
#=============================================================================

UpdateSlaveFW()
{
	# Counter for no. of FW files needs update
   	FWCount=0
	mkdir -p $SLAVEFILEDIR 
	if [ $? -ne 0 ]; then
		ExitOnError "Creating $SLAVEFILEDIR directory"
	fi
	
	for CurrName in ${CurrFWName[@]}
        do
		SlaveFound=0
                TmpIndex=0
                for TmpName in ${TmpFWName[@]}
                do
                        if [ $CurrName == $TmpName ]; then
				SlaveFound=1
                                GetCurrFWIndex $CurrName
                                CurrIndex=$?
                                Return=$(CompareVersion ${CurrFWVer[$CurrIndex]} ${TmpFWVer[$TmpIndex]})
                                if [ $Return == "LOWER" ]; then
                                        ((FWCount++))
                                        Log "Updating $CurrName from ${CurrFWVer[$CurrIndex]} to ${TmpFWVer[$TmpIndex]}"
                                        cp $TMPFIRMWAREDIR/$CurrName $SLAVEFILEDIR
					if [ $? -ne 0 ]; then
						RollbackFWAndSW
						ExitOnError "Copying $CurrFWName to $SlaveSWUpdateFiles"
					fi

					# Update the SW_Version.xml with latest version
					sed '/'$CurrName'/{s/'${CurrFWVer[$CurrIndex]}'/'${TmpFWVer[$TmpIndex]}'/}'\
                                        $SWVERFILE > $TMPSWVERSIONFILE
                                        cp $TMPSWVERSIONFILE $SWVERFILE
					if [ $? -ne 0 ]; then
						RollbackFWAndSW
						ExitOnError "Copying $TMPSWVERSIONFILE to $SWVERFILE"
					fi
                                fi
                        fi
                        ((TmpIndex++))
                done
		
		if [ $SlaveFound -eq 0 ]; then
			Log "$CurrName not present at $UPDATEHWVERFILE"
		fi	
        done

	if [ $FWCount -eq 0 ]; then
		Log "None of Slave FW needs update"
	 else
	 	FWUpdateStatus=$(ExecuteSlaveFWUpdate)
		if [ "$FWUpdateStatus" == "True" ]; then
			Log "Slave FW updated"
		else
			RollbackFWAndSW
			ExitOnError "Slave FW update"
		fi
	fi
}

#===To do: FUNCTION ==========================================================
# NAME: ExecuteSlaveFWUpdate
# DESCRIPTION: To execute SlaveFWUpdateSW 
# PARAMETER : NA 
#=============================================================================

ExecuteSlaveFWUpdate()
{
	Log "Need to implement" 
	# To execute SlaveFWUpdateSW with $SLAVEFILEDIR as parameter		 
	echo "True"
}

#=== FUNCTION ================================================================
# NAME: RollbackFWAndSW
# DESCRIPTION: Rollback on SW Update error 
# PARAMETER : NA 
#=============================================================================

RollbackFWAndSW()
{
	Log "Performing Rollback"
	rm -r $SLAVEFILEDIR/* || ExitOnError "Deleting $SLAVEFILEDIR failed"
	
	cp -r $ROLLBACKDIR/Firmware/ $SLAVEFILEDIR	
	if [ $? -ne 0 ]; then
		ExitOnError "Error in copying $ROLLBACKDIR/Firmware/ to $SLAVEFILEDIR"
	fi

	 	FWUpdateStatus=$(ExecuteSlaveFWUpdate)
		if [ "$FWUpdateStatus" == "True" ]; then
			Log "Slave FW Rollback passed"
		else
			Log "Slave FW Rollback failed"
		fi

	RollbackSW	
}

#=== FUNCTION ================================================================
# NAME: RollbackSW
# DESCRIPTION: To perform Rollback of SW on error 
# PARAMETER : NA 
#=============================================================================

RollbackSW()
{
	Log "Performing SW Rollback"
	cp -r $ROLLBACKDIR/bin/ $ROOTDIR/
	if [ $? -ne 0 ]; then
		ExitOnError "Error in copying $ROLLBACKDIR/bin/ to $ROOTDIR/"
	fi

	cp $ROLLSWVERFILE $SETTINGDIR/
	if [ $? -ne 0 ]; then
                ExitOnError "Error in copying $ROLLSWVERFILE to $SETTINGDIR"
        fi
}

#=== FUNCTION ================================================================
# NAME: UpdateRollback
# DESCRIPTION: To Update Rollback folder after update process is successful 
# PARAMETER : NA 
#=============================================================================

UpdateRollback()
{
	Log "Updating Rollback folder"
	Clean
	cp -r $BINDIR/ $FIRMWAREDIR/ $SETTINGDIR/ $ROLLBACKDIR/
	if [ $? -ne 0 ]; then
                ExitOnError "Error in copying $BINDIR/ $FIRMWAREDIR/ $SETTINGDIR/ to $ROLLBACKDIR"
        fi
}


#=== FUNCTION ===========================================================================
# NAME: UpdateSWBinaries
# DESCRIPTION:  Check for the higher version of SW binaries available at update package.
#               Updating the current SW binaries with higher version available 
#               Also updates the SW_Version.xml file
# PARAMETER : NA 
#========================================================================================

UpdateSWBinaries()
{
	Log "Updating higher version of SW files from SW package"

	# Counter for no. of SW files need to be updated
	SWCount=0		

	ReadXMLintoArray
	
	for CurrName in ${CurrSWName[@]} 
        do
                TmpIndex=0
		BinFound=0
		for TmpName in ${TmpSWName[@]}
                do
			if [ $CurrName == $TmpName ]; then
				BinFound=1
				GetCurrSWIndex $CurrName
                                CurrIndex=$?
				Return=$(CompareVersion ${CurrSWVer[$CurrIndex]} ${TmpSWVer[$TmpIndex]})
                                if [ $Return == "LOWER" ]; then
					((SWCount++))
                                        Log "Updating $CurrName from ${CurrSWVer[$CurrIndex]} to ${TmpSWVer[$TmpIndex]}"
					cp $TMPBINDIR/$CurrName $BINDIR/$CurrName  &>/dev/null
					if [ $? -ne 0 ]; then
						RollbackSW
						ExitOnError "Error in copying $TMPBINDIR/$CurrName to $BINDIR" 
					fi
					sed '/'$CurrName'/{s/'${CurrSWVer[$CurrIndex]}'/'${TmpSWVer[$TmpIndex]}'/}'\
                                        $SWVERFILE > $TMPSWVERSIONFILE 
					cp $TMPSWVERSIONFILE $SWVERFILE
					if [ $? -ne 0 ]; then
						RollbackSW
						ExitOnError "Copying $TMPSWVERSIONFILE to $SWVERFILE" 
					fi
                                fi
                        fi
                	((TmpIndex++))
        	done

		if [ $BinFound -eq 0 ]; then
			Log "$CurrName not present at $UPDATESWVERFILE"
		fi
		
	done
		
	if [ $SWCount -eq 0 ]; then
		Log "None of SW file needs update"
	fi
	
	Log "SW Update: finished; Starting Slave FW Update process"
}



#=== FUNCTION ================================================================
# NAME: GetCurrSWIndex
# DESCRIPTION: To find index position of SW file name in array
# PARAMETER 1: SW Filename
#=============================================================================

GetCurrSWIndex()
{
	count=${#CurrSWName[@]} 
	index=0
	
	while [ "$index" -lt "$count" ]
	do
        	if [ "${CurrSWName[$index]}" == "$1" ]; then
                	return $index
	        fi
        	((index++))
	done
}

#=== FUNCTION ================================================================
# NAME: GetCurrFWIndex
# DESCRIPTION: To find index position of FW file name in array
# PARAMETER 1: FW Filename 
#=============================================================================

GetCurrFWIndex()
{
        count=${#CurrFWName[@]}
        index=0

        while [ "$index" -lt "$count" ]
        do
                if [ "${CurrFWName[$index]}" == "$1" ]; then
                        return $index
                fi
                ((index++))
        done
}

#=== FUNCTION ================================================================
# NAME: ReadXMLintoArray
# DESCRIPTION:  To Read XML file for SW Name and Version, from current device and 
#		update package
# PARAMETER : NA 
#=============================================================================

ReadXMLintoArray()
{
	if [ ! -f $SWVERFILE ]; then
		ExitOnError "$SWVERFILE not present"
	fi

	if [ ! -f $UPDATESWVERFILE ]; then
		ExitOnError "$UPDATESWVERFILE not present"
	fi

	ReadXML $SWVERFILE
        CurrSWName=("${SWNAME[@]}")
        CurrSWVer=("${SWVER[@]}")
        CurrFWName=("${FWNAME[@]}")
        CurrFWVer=("${FWVER[@]}")

        ReadXML $UPDATESWVERFILE
        TmpSWName=("${SWNAME[@]}")
        TmpSWVer=("${SWVER[@]}")
        TmpFWName=("${FWNAME[@]}")
        TmpFWVer=("${FWVER[@]}")
}



#=== FUNCTION ================================================================
# NAME: ReadXML 
# DESCRIPTION: To read the xml file into Array for filename and version
# PARAMETER 1: SW_Version.xml path
#=============================================================================

ReadXML()
{
        ClearTempFile
        ReadNamesFromXML Master_SW Filename $1
        SWNAME=( $(cat $TMPDIR/temp.txt) )

        ClearTempFile
        ReadVersionFromXML Master_SW Version $1
        SWVER=( $(cat $TMPDIR/temp.txt) )

        ClearTempFile
        ReadNamesFromXML Firmware Filename $1
        FWNAME=( $(cat $TMPDIR/temp.txt) )

        ClearTempFile
        ReadVersionFromXML Firmware Version $1
        FWVER=( $(cat $TMPDIR/temp.txt) ) 

	rm $TMPDIR/temp.txt
}

#=== FUNCTION ================================================================
# NAME: ReadVersionFromXML
# DESCRIPTION: To extract file versions from xml file 
# PARAMETER 1: Master_SW or Firmware
# PARAMETER 2: Version
# PARAMETER 3: SW_Version.xml file path 
#=============================================================================

ReadVersionFromXML()
{
        for i in $(sed -n '/'$1'/,/\/'$1'/p' $3 | grep file)
        do
                echo "$i" | sed -n '/'$2'="/,/\"/p' | cut -d'"' -f2 | cut -d"_" -f2 1>>$TMPDIR/temp.txt
        done
}

#=== FUNCTION ================================================================
# NAME: ReadNamesFromXML
# DESCRIPTION: To extract file names from xml file 
# PARAMETER 1: Master_SW or Firmware
# PARAMETER 2: Filename
# PARAMETER 3: SW_Version.xml file path
#=============================================================================

ReadNamesFromXML()
{
        for i in $(sed -n '/'$1'/,/\/'$1'/p' $3 | grep file)
        do
                echo "$i" | sed -n '/'$2'="/,/\"/p' | cut -d'"' -f2 1>>$TMPDIR/temp.txt
        done
}

#=== FUNCTION ================================================================
# NAME: ClearTempFile
# DESCRIPTION: To clear temp file used for parsing xml file 
# PARAMETER : NA 
#=============================================================================

ClearTempFile()
{
        echo -n "" > $TMPDIR/temp.txt
}

#==============================================================
# Script Begins from Here
# -update:  	Update the latest SW and Slave FW files
# -updateRollback: Update the rollback folder with new SW and 
#	    	   Slave FW files
#==============================================================

[ ! $# -eq 1 ] && { Log "$CMDARG"; exit 1; }

if [ $1 = "-update" ]; then
        	
	Log "Update process started"
	MasterSWUpdate
	Log "Update Status: Passed"

elif [ $1 = "-updateRollback" ]; then
	UpdateRollback
	
else
	Log "$CMDARG"
	exit 1
fi	

exit 0

