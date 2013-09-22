#!/bin/bash

#/****************************************************************************/
#/*! \file MasterSWUpdate.sh
 #
 #  \brief: 	This updates the new Software binaries and Slave Firmware also
 #		rollback the binaries and firmware on error.
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
#/****************************************************************************/

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

CMDARG="Usage: $0 [-update] [-updateRollback] [baseeventid]"

# Clean Temp file on Abort or terminate
trap Clean ABRT TERM  

# Rollback on keyboard interrupt
trap Rollback INT

# to declare associative arrays it is require to use -A option
# to store all current software versions - reads from Settings/SW_Version.xml
declare -A CurrSW
# to store current firmware versions - reads from Settings/SW_Version.xml
declare -A CurrFW
# to store temporary software versions - reads from tmp/Settings/SW_Version.xml
declare -A TmpSW
# to store temporary firmware versions - reads from tmp/Settings/SW_Version.xml
declare -A TmpFW

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

                ReadXMLintoArray
		RetVal=$?
                if [  $RetVal -eq 0 ]; then
			UpdateSWBinaries
			UpdateSlaveFW

		else

			Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$TMPBINDIR/" "$ROOTDIR/"

			# Updating all SW binaries
			cp -r $TMPBINDIR/ $ROOTDIR/
			if [ $? -ne 0 ]; then
				RollbackSW
				ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$TMPBINDIR/" "$ROOTDIR/" 
			fi

			[ -f $BINDIR/$PROGNAME ] && rm $BINDIR/$PROGNAME			

			# Creating temp directory and copying all FW files for update
			mkdir -p $SLAVEFILEDIR 
			if [ $? -ne 0 ]; then
				RollbackFWAndSW
				ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_CREATION_FAILED" "$SLAVEFILEDIR" 
			fi
                        
			Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$TMPFIRMWAREDIR/" "$SLAVEFILEDIR"

			cp -r $TMPFIRMWAREDIR/* $SLAVEFILEDIR 
			if [ $? -ne 0 ]; then
				ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$TMPFIRMWAREDIR" "$SLAVEFILEDIR" 
			fi

			Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$UPDATESWVERFILE" "$SETTINGDIR/"

			cp $UPDATESWVERFILE $SETTINGDIR/
			if [ $? -ne 0 ]; then
				RollbackFWAndSW
				ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$UPDATESWVERFILE" "$SETTINGDIR" 
			fi	
		        
			FWUpdateStatus=$(ExecuteSlaveFWUpdate)
			if [ "$FWUpdateStatus" != "True" ]; then				
				RollbackFWAndSW				
				ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_SLAVE_UPDATE_FAILED"
			else
				# IN1301 fix		
				cp -r $SLAVEFILEDIR/* $FIRMWAREDIR
				if [ $? -ne 0 ]; then
					ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$SLAVEFILEDIR" "$FIRMWAREDIR"
				fi
			fi

		fi
        else
        	ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS" "$ROLLBACKDIR"
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
	if [ -e $SLAVEFILEDIR ]; then
		rm -r $SLAVEFILEDIR
	fi

	mkdir -p $SLAVEFILEDIR 
	if [ $? -ne 0 ]; then
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_CREATION_FAILED" "$SLAVEFILEDIR"
	fi

	for Name in "${!TmpFW[@]}";do		
		# if the name does not exist in the associative array then it returns nothing		
		if [ "${CurrFW[$Name]}" == "" ]; then
			Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_MISSING_XML_ATTR" "$Name" "$UPDATESWVERFILE" "$SWVERFILE"
		else		
			Return=$(CompareVersion ${CurrFW[$Name]} ${TmpFW[$Name]})
		        if [ $Return == "LOWER" ]; then
		                ((FWCount++))
		                Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$TMPFIRMWAREDIR/$Name" "$SLAVEFILEDIR" 
		                cp $TMPFIRMWAREDIR/$Name $SLAVEFILEDIR
				if [ $? -ne 0 ]; then
					RollbackFWAndSW
					ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$TMPFIRMWAREDIR/$Name" "$SLAVEFILEDIR"
				fi

				# Update the SW_Version.xml with latest version
				sed '/'$Name' *"/{s/'${CurrFW[$Name]}'/'${TmpFW[$Name]}'/}'\
		                $SWVERFILE > $TMPSWVERSIONFILE
		                cp $TMPSWVERSIONFILE $SWVERFILE
				if [ $? -ne 0 ]; then
					RollbackFWAndSW
					ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$TMPSWVERSIONFILE" "$SWVERFILE"
				fi
		        fi			
		fi
	done	
	

	if [ $FWCount -ne 0 ]; then		
	 	FWUpdateStatus=$(ExecuteSlaveFWUpdate)
		if [ "$FWUpdateStatus" != "True" ]; then			
			RollbackFWAndSW
			ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_SLAVE_UPDATE_FAILED"
		else
			# IN1301 fix		
			cp -r $SLAVEFILEDIR/* $FIRMWAREDIR
			if [ $? -ne 0 ]; then
		        	ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$SLAVEFILEDIR" "$FIRMWAREDIR"
			fi
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
	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_EXECUTING_COMPONENT" "$PTSFILE"
	# To execute SlaveFWUpdateSW with $SLAVEFILEDIR as parameter
	# IN1303 fix
	SlaveUpdatePar=""
	for fm in `ls $SLAVEFILEDIR`
	do
		if echo "$fm" | grep ASB3 >/dev/null; then
			SlaveUpdatePar=$SlaveUpdatePar" 3 "$SLAVEFILEDIR/$fm
		fi
		if echo "$fm" | grep ASB5 >/dev/null; then
			SlaveUpdatePar=$SlaveUpdatePar" 5 "$SLAVEFILEDIR/$fm
		fi
		if echo "$fm" | grep ASB15 >/dev/null; then
			SlaveUpdatePar=$SlaveUpdatePar" 15 "$SLAVEFILEDIR/$fm
		fi
	done
	#ReturnValue="$(timeout "$PTS_TIMEOUT" "$PTSFILE" "$SLAVEUPDATEFILE" "$BASE_EVENT_ID")"
	timeout "$PTS_TIMEOUT" "$PTSFILE" "$SlaveUpdatePar" >/dev/null
	ReturnValue=$?
	if [ $ReturnValue -eq 0 ]; then
		Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_PTS_SUCCESS"
		echo "True" 
	else
		Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_PTS_FAILED"
		echo "False"
	fi	
}

#=== FUNCTION ================================================================
# NAME: RollbackFWAndSW
# DESCRIPTION: Rollback on SW Update error 
# PARAMETER : NA 
#=============================================================================

RollbackFWAndSW()
{	
	rm -r $SLAVEFILEDIR/* || ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_DELETION_FAILED" "$SLAVEFILEDIR"

	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_ROLLINGBACK" "$SLAVEFILEDIR" "$ROLLBACKFIRMWAREDIR/"

	cp -r $ROLLBACKFIRMWAREDIR/* $SLAVEFILEDIR	
	if [ $? -ne 0 ]; then
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$ROLLBACKFIRMWAREDIR/" "$SLAVEFILEDIR"
	fi

 	FWUpdateStatus=$(ExecuteSlaveFWUpdate)
	if [ "$FWUpdateStatus" != "True" ]; then			
		Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_SLAVE_UPDATE_FAILED"
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
	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_ROLLINGBACK" "$ROOTDIR/" "$ROLLBACKBINDIR/"
	cp -r $ROLLBACKBINDIR/ $ROOTDIR/
	if [ $? -ne 0 ]; then
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$ROLLBACKBINDIR/" "$ROOTDIR/"
	fi
	
	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_ROLLINGBACK" "$SETTINGDIR/" "$ROLLSWVERFILE" 
	cp $ROLLSWVERFILE $SETTINGDIR/
	if [ $? -ne 0 ]; then
                ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$ROLLSWVERFILE" "$SETTINGDIR"
        fi
}

#=== FUNCTION ================================================================
# NAME: UpdateRollback
# DESCRIPTION: To Update Rollback folder after update process is successful 
# PARAMETER : NA 
#=============================================================================

UpdateRollback()
{	
	Clean
	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$BINDIR/" "$ROLLBACKDIR"
	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$FIRMWAREDIR/" "$ROLLBACKDIR"
	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$SETTINGDIR/" "$ROLLBACKDIR"

	cp -r $BINDIR/ $FIRMWAREDIR/ $SETTINGDIR/ $ROLLBACKDIR/
	if [ $? -ne 0 ]; then
                ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$BINDIR/, $FIRMWAREDIR/,  $SETTINGDIR/" "$ROLLBACKDIR"
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
	# Counter for no. of SW files need to be updated
	SWCount=0
	
	for Name in "${!TmpSW[@]}";do
		# if the name does not exist in the associative array then it returns nothing
		if [ "${CurrSW[$Name]}" == "" ]; then
			Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_MISSING_XML_ATTR" "$Name" "$UPDATESWVERFILE" "$SWVERFILE"
		else		
			Return=$(CompareVersion ${CurrSW[$Name]} ${TmpSW[$Name]})
			if [ $Return == "LOWER" ]; then
				((SWCount++))
		                Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$TMPBINDIR/$Name" "$BINDIR/$Name"
				cp $TMPBINDIR/$Name $BINDIR/$Name  &>/dev/null
				if [ $? -ne 0 ]; then
					RollbackSW
					ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$TMPBINDIR/$Name" "$BINDIR/$Name" 
				fi
				sed '/'$Name' *"/{s/'${CurrSW[$Name]}'/'${TmpSW[$Name]}'/}'\
		                $SWVERFILE > $TMPSWVERSIONFILE 
				cp $TMPSWVERSIONFILE $SWVERFILE
				if [ $? -ne 0 ]; then
					RollbackSW
					ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$TMPSWVERSIONFILE" "$SWVERFILE" 
				fi
		        fi
		fi
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

	# check whether SW_Version.xml file exists in the TMP directory, if not don't proceed further			
	if [ ! -f $UPDATESWVERFILE ]; then
		ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS" "$UPDATESWVERFILE"
	fi	

	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_PARSING_XML" "$UPDATESWVERFILE"

        ReadXML $UPDATESWVERFILE

        TmpSWName=(""$SWNAME"")
        TmpSWVer=(""$SWVER"")

	Index=0
	# store the data in associative array using the key value(SW name)
	# for this array we need not require to check the name because
	# this data will be read from the zip file so if any problem 
	# occurs to the unzip file then it will abort the program
	for Name in "${TmpSWName[@]}";do
		if [ "$Name" == "" ]; then			
			ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED" "$UPDATESWVERFILE"
		fi		
		TmpSW[$Name]=${TmpSWVer[$Index]}
		((Index++))
	done

        TmpFWName=(""$FWNAME"")	
        TmpFWVer=(""$FWVER"")

	Index=0
	# store the data in associative array using the key value(FW name)
	for Name in "${TmpFWName[@]}";do
		if [ "$Name" == "" ]; then			
			ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED" "$UPDATESWVERFILE"
		fi
		TmpFW[$Name]=${TmpFWVer[$Index]}
		((Index++))
	done

	# check whether SW_Version.xml file exists or not
	if [ ! -f $SWVERFILE ]; then
		Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_SWVERSION_FILE_NOT_FOUND" "$SWVERFILE"
		return 1
	fi

	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_PARSING_XML" "$SWVERFILE"

	ReadXML $SWVERFILE

	# converting the string to array
	CurrSWName=(""$SWNAME"")	
        CurrSWVer=(""$SWVER"")
	
	Index=0
	
	# store the data in associative array using the key value(SW name)
	for Name in "${CurrSWName[@]}";do
		# check the name exists in the array as empty or not, if empty don't create array
		if [ "$Name" != "" ]; then			
			CurrSW[$Name]=${CurrSWVer[$Index]}
		else
			Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_SWVERSION_FILE_CORRUPTED" "$SWVERFILE"
			return 1
		fi
		((Index++))
	done
	CurrFWName=(""$FWNAME"")	
	CurrFWVer=(""$FWVER"")	

	Index=0
	# store the data in associative array using the key value(FW name)
	for Name in "${CurrFWName[@]}";do
		# check the name exists in the array as empty or not, if empty don't create array
		if [ "$Name" != "" ]; then			
			CurrFW[$Name]=${CurrFWVer[$Index]}
		else
			Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_SWVERSION_FILE_CORRUPTED" "$SWVERFILE"
			return 1
		fi
		((Index++))
	done

	return 0
}



#=== FUNCTION ================================================================
# NAME: ReadXML 
# DESCRIPTION: To read the xml file into Array for filename and version
# PARAMETER 1: SW_Version.xml path
#=============================================================================

ReadXML()
{        
	# IN1425 fix
	SWNAME="$(xmlstarlet sel -t -m '(/SW_Version/Master_SW/file)' -v @Filename -n  "$1" 2>/dev/null)"

        SWVER="$(xmlstarlet sel -t -m '(/SW_Version/Master_SW/file)' -v @Version -n  "$1" 2>/dev/null | cut -d'_' -f2)"

        FWNAME="$(xmlstarlet sel -t -m '(/SW_Version/Firmware/file)' -v @Filename -n  "$1" 2>/dev/null)"

        FWVER="$(xmlstarlet sel -t -m '(/SW_Version/Firmware/file)' -v @Version -n  "$1" 2>/dev/null | cut -d'_' -f2)"

}


#==============================================================
# Script Begins from Here
# -update:  	Update the latest SW and Slave FW files
# -updateRollback: Update the rollback folder with new SW and 
#	    	   Slave FW files
#==============================================================

[ ! $# -eq 2 ] && { ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_INVALID_CMD_ARGS" "$CMDARG"; }
# Second argument will be  event ID
FirstArgument=$1
if [ $FirstArgument = "-update" ]; then
	BASE_EVENT_ID=$2
	IsUpdateStarted=true	
	MasterSWUpdate
	UpdateRebootFile "Success"
	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_SUCCESS"	

elif [ $FirstArgument = "-updateRollback" ]; then	
	BASE_EVENT_ID=$2	
	UpdateRollback	
else
	ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_INVALID_CMD_ARGS" "$CMDARG"	
fi	

exit 0

