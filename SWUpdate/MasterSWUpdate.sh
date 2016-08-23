#!/bin/bash

#/****************************************************************************/
#/*! \file MasterSWUpdate.sh
 #
 #  \brief: Implementation for Software update.
 #              -Updates Software binaries
 #              -Updates Settings & Translations
 #	            -Init Scripts
 #              -Firmware
 #              -Slave board options
 #              -Hanldes errors occuring in above steps
 #
 #   $Version: $ 1.2
 #   $Date:    $ 2014-03-03
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
#/****************************************************************************/

# Extract directory path from where this script is executing
CURRDIR="$( cd "$( dirname "$0" )" && pwd )"

if [ -f $CURRDIR/Utils.sh ]; then
	. $CURRDIR/Utils.sh
else
	echo "$CURRDIR/Utils.sh file missing"
	exit 1
fi
if [ -f $CURRDIR/ProjectSpecificConfig.sh ]; then
	. $CURRDIR/ProjectSpecificConfig.sh
else
	echo "$CURRDIR/ProjectSpecificConfig.sh file missing"
	exit 1
fi

# jeff@2016-08-23@replace 'MasterSWupdateStarter.sh' first
replace_updater()
{
    cp -rf $CURRDIR/MasterSWupdateStarter.sh '/home/Leica/Bin'
    cd '/home/Leica/Bin'
    local FileName='MasterSWupdateStarter.sh'
    local Md5Sum=$(md5sum $FileName)
    sed -i "s@^.*$FileName.*\$@$Md5Sum@" .md5sum.txt
    cd - > /dev/null 2>&1
}
replace_updater

# Extract current script name PROGNAME=$(basename $0) 

CMDARG="Usage: $0 [-update / -updateRollback] [baseeventid]"

# Clean Temp file on Abort or terminate
#trap Clean ABRT TERM  
# USR1 signal is sent from powerfailmonitor script. On reception exit.
# we don't clean up becuase we need to tmp folder for rollback
#trap "PowerFailed;exit 1" USR1  

# to store all current software versions - read from Settings/SW_Version.xml
declare -A CurrSWNameVersionMap
# to store current firmware versions - read from Settings/SW_Version.xml
declare -A CurrFWNameVersionMap
# to store temporary software versions - read from tmp/Settings/SW_Version.xml
declare -A UpdatePkgSWNameVersionMap
# to store temporary firmware versions - read from tmp/Settings/SW_Version.xml
declare -A UpdatePkgFWNameVersionMap

# to store temporary  versions of "InitScripts" section- read from tmp/Settings/SW_Version.xml
declare -A UpdatePkgInitScriptsNameVersionMap
# to store current versions of "InitScripts" - read from Settings/SW_Version.xml
declare -A CurrInitScriptsNameVersionMap

# to store temporary  versions of "BoardOptions" section- read from tmp/Settings/SW_Version.xml
declare -A UpdatePkgBoardOptionNameVersionMap
# to store current versions of "BoardOptions" - read from Settings/SW_Version.xml
declare -A CurrBoardOptionNameVersionMap

#Indicates if the software binaries got updated.Doesnt indicated if update itself was success or failure
SwUpdated=false
#Indicates if firmware got updated.Doesnt indicated if update itself was success or failure
FwUpdated=false
#Indicates if init scripts got updated.Doesnt indicated if update itself was success or failure
InitScriptsUpdated=false


#=== FUNCTION ================================================================
# NAME: LogRollBackAndExit
# DESCRIPTION:  Logs error, Rollbacks required module and exits with error 
# PARAMETER 1: RollbackFlag = "SW" /"FW"/"BoardOptions"/"All"
# PARAMETER 2..N:Arguments for logging
#=============================================================================
LogRollBackAndExit()
{
    local RollbackFlag=$1
    #Shift argument list by one , i.e $2 arg will become 1st , $3 will become $2 and so on...
    shift 
    Log "$EVENT_SOURCE_MASTER" "$@"
    Rollback $RollbackFlag

    if [ $InitScriptsUpdated = "true" ]; then
        #changes in init scripts need an reboot to take effect
        ExitAndReboot 
    else
        ExitOnError
    fi
}

#=== FUNCTION ================================================================
#NAME: VerifyMd5sum
#DESCRIPTION: Verifies Md5sum of the directory passed as argument
#PARAMETER: $1 -> Directory for which md5sum needs to checked
#RETURN:0 on md5sum check success, else 1
#=============================================================================
VerifyMd5sum()
{
    cd "$1"
    md5sum -c .md5sum.txt >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        cd - >/dev/null 2>&1 
        return 1
    else
        cd - >/dev/null 2>&1 
        return 0
    fi
}

#=== FUNCTION ================================================================
#NAME: CheckIfAnyModulesWereUpdated
#DESCRIPTION: Checks if S/W update was performed
#PARAMETER:Na
#RETURN:0 means one of the modules was updated, else 1
#=============================================================================
CheckIfAnyModulesWereUpdated()
{
    if [[ ( $SwUpdated = "true" ) || ( $FwUpdated = "true" ) || ( $InitScriptsUpdated = "true" ) ]] ; then
        return 0
    else
        return 1
    fi
}
#=== FUNCTION ================================================================
# NAME: UpdateMd5SumForSWVersionXml
# DESCRIPTION: Updates Md5sum for SW version xml
# PARAMETER : NA 
#=============================================================================
UpdateMd5SumForSWVersionXml()
{
    local SWVersionFileName=$(basename $SWVERFILE)
    cd $SETTINGDIR
    local SWVersionMd5sum=$(md5sum $SWVersionFileName)
    sed -i "/\s$SWVersionFileName\b/c\\$SWVersionMd5sum" $SETTINGDIR/.md5sum.txt
    cd - >/dev/null 2>&1
}

#=== FUNCTION ================================================================
# NAME: UpdateMd5SumForSettings
# DESCRIPTION: Updates Md5sum for Settings directory
# PARAMETER : NA 
#=============================================================================
UpdateMd5SumForSettings()
{
    local Line
    while read Line; do
        local FileName=$(echo "$Line" | awk '{print $2}')
        CheckIfMd5SumNeedsToBeUpdatedForFileInSettings $FileName
        [ $? -eq 0 ] && sed -i "/\s$FileName\b/c\\$Line" $SETTINGDIR/.md5sum.txt
    done < $TMPSETTINGDIR/.md5sum.txt
    UpdateMd5SumForSWVersionXml
    VerifyMd5sum "$SETTINGDIR"
    [ $? -ne 0 ] && LogRollBackAndExit "SW" "$EVENT_SWUPDATE_ERROR_MD5SUM" "$SETTINGDIR"
}

#=== FUNCTION ================================================================
# NAME: UpdateMd5SumForBin
# DESCRIPTION: Updates Md5sum for Bin Directory
# PARAMETER : NA 
#=============================================================================
UpdateMd5SumForBin()
{
    local Line
    while read Line; do
        local FileName=$(echo "$Line" | awk '{print $2}')
        CheckIfMd5SumNeedsToBeUpdateForFileInBin $FileName
        [ $? -eq 0 ] && sed -i "/\s$FileName\b/c\\$Line" $BINDIR/.md5sum.txt
    done < $TMPBINDIR/.md5sum.txt
    VerifyMd5sum "$BINDIR"
    [ $? -ne 0 ] && LogRollBackAndExit "SW" "$EVENT_SWUPDATE_ERROR_MD5SUM" "$BINDIR"
}

#=== FUNCTION ================================================================
# NAME: UpdateMd5SumForFirmware
# DESCRIPTION: Updates Md5sum for Firmware Directory
# PARAMETER : NA 
#=============================================================================
UpdateMd5SumForFirmware()
{
    local Line
    while read Line; do
        local FileName=$(echo "$Line" | awk '{print $2}')
        CheckIfMd5SumNeedsToBeUpdateForFileInFirmware $FileName
        if [ $? -eq 0 ]; then
			sed -i "/\s$FileName\b/c\\$Line" $FIRMWAREDIR/.md5sum.txt
			cp $TMPFIRMWAREDIR/$FileName $FIRMWAREDIR/
		fi
    done < $TMPFIRMWAREDIR/.md5sum.txt
    VerifyMd5sum "$FIRMWAREDIR"
    [ $? -ne 0 ] && LogRollBackAndExit "FW" "$EVENT_SWUPDATE_ERROR_MD5SUM" "$FIRMWAREDIR"
}

#=== FUNCTION ================================================================
# NAME: CheckIfMd5SumNeedsToBeUpdateForFileInFirmware
# DESCRIPTION: checks if a firmware binary got updated, which means we need to
#              update its md5sum
# PARAMETER : NA 
#=============================================================================
CheckIfMd5SumNeedsToBeUpdateForFileInFirmware()
{
    local FileName
    for FileName in "${UPDATED_FIRMWARE[@]}"; do
        [[ "$1" = "$FileName" ]] && return 0 || continue
    done
    return 1
}

#=== FUNCTION ================================================================
# NAME: CheckIfMd5SumNeedsToBeUpdateForFileInBin
# DESCRIPTION: checks if a Software binary got updated, which means we need to
#              update its md5sum
# PARAMETER : NA 
#=============================================================================
CheckIfMd5SumNeedsToBeUpdateForFileInBin()
{
    local FileName
    for FileName in "${UPDATED_SW_BINARIES[@]}"; do
        [[ "$1" = $FileName ]] && return 0 || continue
    done 
    return 1
}


#=== FUNCTION ================================================================
# NAME: CheckIfMd5SumNeedsToBeUpdatedForFileInSettings
# DESCRIPTION: checks if a Settings File got updated, which means we need to
#              update its md5sum
# PARAMETER 1: File in Setting directory
#=============================================================================
CheckIfMd5SumNeedsToBeUpdatedForFileInSettings()
{
    [[ ${READONLY_OR_INSTRUMENT_SPECIFIC_FILES[*]} =~ $1 ]] && return 1
    local Directory
    for Directory in ${READONLY_OR_INSTRUMENT_SPECIFIC_DIR_LIST[@]}; do
         [[ $1 =~ $Directory/ ]] && return 1
    done
    #Md5sum needs to be updated
    return 0
}

#=== FUNCTION ===================================================================
# NAME: UpdateSlaveFW
# DESCRIPTION: 	Check for the higher version of firmware available at package.
#		Copy the higher version firmware files to temp directory. 
#		Execute Slave FW Update SW to update required Slave device
#		Also updates the SW_Version.xml file 
# PARAMETER : NA 
#=============================================================================
UpdateSlaveFW()
{
	# Counter for no. of FW files needs update
    local FWCount=0
	mkdir -p $SLAVEFILEDIR 
	[ $? -ne 0 ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_CREATION_FAILED" "$SLAVEFILEDIR"
   
    #Create a list of firmwares which need update with respect to the version number
	for Name in "${!UpdatePkgFWNameVersionMap[@]}";do	#Name would contain firmware names per ASB. e.g. ASB2_0.bin, ASB2_2.bin	
	    Return=$(CompareVersion ${CurrFWNameVersionMap[$Name]} ${UpdatePkgFWNameVersionMap[$Name]})
		if [ "$Return" = "LOWER" ]; then
            FIRMWARES_TO_BE_UPDATED+=( "$Name" )
        fi        
    done
    FilterUnAvailableASBs
    for Name in "${FIRMWARES_TO_BE_UPDATED[@]}";do
        ((FWCount++))
        local FirmwareName=$(GenerateFirmwareName $Name)
        Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE"  "$TMPFIRMWAREDIR/$FirmwareName" "$SLAVEFILEDIR/$Name" 
        Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_UPDATING_FILE" "$Name" "${CurrFWNameVersionMap[$Name]}" "${UpdatePkgFWNameVersionMap[$Name]}"
        cp  $TMPFIRMWAREDIR/$FirmwareName $SLAVEFILEDIR/$Name
        [ $? -ne 0 ] && LogRollBackAndExit "FW" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$TMPFIRMWAREDIR/$Name" "$SLAVEFILEDIR"
        UPDATED_FIRMWARE+=( "$FirmwareName" )
    done    
	if [ $FWCount -ne 0 ]; then 
        ExecutePTS "$SLAVEFILEDIR"
        for Name in "${FIRMWARES_TO_BE_UPDATED[@]}";do
            #Update Version in SW_Version.xml    
            xmlstarlet ed -L -u "//file[@Filename='$Name']/@Version" -v ${UpdatePkgFWNameVersionMap[$Name]} $SWVERFILE
        done
        UpdateMd5SumForFirmware
        UpdateMd5SumForSWVersionXml
        VerifyMd5sum "$SETTINGDIR"
        [ $? -ne 0 ] && LogRollBackAndExit "FW" "$EVENT_SWUPDATE_ERROR_MD5SUM" "$SETTINGDIR"
    fi
}

#=== FUNCTION ================================================================
# NAME: GenerateFirmwareName
# DESCRIPTION: Converts firmware binary name of the format ASBX_Y.bin to
#              ASBX.bin. e.g ASB13_0.bin to ASB13.bin
# PARAMETER 1: Fimrware binary  name in the format ASBX_Y.bin
#=============================================================================
GenerateFirmwareName()
{
    #Hack - Legacy naming convention 
    if [[ "$1" =~ "ASB8_0" ]]; then
        local  FirmwareName="ASB8a.bin"
    elif [[ "$1" =~ "ASB9_0" ]]; then
        local  FirmwareName="ASB8b.bin"
    elif [[ "$1" =~ "ASB6_0" ]]; then
        local  FirmwareName="SABa.bin"
    elif [[ "$1" =~ "ASB7_0" ]]; then
        local  FirmwareName="SABb.bin"
    else
        #local  FirmwareName=$(echo $1 | awk -F_ '{ print $1 }').bin # e,g, ASB13_0.bin will become ASB13.bin
        local  FirmwareName=$(echo $1 | awk -F_ '{ print $1 }') # The bin now is ASB3.bin
    fi
    echo $FirmwareName
}

#===FUNCTION ==========================================================
# NAME: ExecutePTS
# DESCRIPTION: executes PTS binary which implements the actual slave firmware
#              update 
# PARAMETER 1: Slaveupdate js file/ Board options js file  
# PARAMETER 2: DontRollbackOnFailure="true"/"false"
#=============================================================================
ExecutePTS()
{
    Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_EXECUTING_COMPONENT" "$PTSFILE"
    cd $TMPBINDIR
    "./${PTSFILE##*/}" "$1" "$BASE_EVENT_ID" -qws > /dev/null 2>&1
    local ReturnValue=$?
    cd - > /dev/null 2>&1
    FwUpdated=true
    if [ $ReturnValue -eq 0 ]; then
        Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_PTS_SUCCESS"
        if [ "$1" = "$SLAVEUPDATEFILE" ]; then 
            for File in "${UPDATED_FIRMWARE[@]}"; do
                Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE"  "$TMPFIRMWAREDIR/$File" "$FIRMWAREDIR" 
                cp "$TMPFIRMWAREDIR"/"$File" "$FIRMWAREDIR/" > /dev/null 2>&1
                if [ $? -ne 0 ]; then
                    if [ "$2" != "DontRollbackOnFailure" ]; then
                        LogRollBackAndExit "FW" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$SLAVEFILEDIR" "$FIRMWAREDIR"
                    else
                        Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$SLAVEFILEDIR" "$FIRMWAREDIR"
                        return 1
                    fi
                fi
            done
        fi
    else
        Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_PTS_FAILED" "$ReturnValue"
        if [ "$2" = "DontRollbackOnFailure" ]; then
            Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_SLAVE_UPDATE_FAILED"
        else
            if [ "$1" = $SLAVEUPDATEFILE ]; then
                local RollbackType="FW" 
            fi
            LogRollBackAndExit "$RollbackType" "$EVENT_SWUPDATE_SLAVE_UPDATE_FAILED"
        fi
    fi
    return $ReturnValue
}


#=== FUNCTION ================================================================
# NAME: RollbackInitScripts 
# DESCRIPTION: Rolls back init scripts from rollback directory
# PARAMETER : NA 
#=============================================================================
RollbackInitScripts()
{
    cp "$ROLLBACKINITSCRIPTS"/* "$EBOXINITSCRIPTSDIR"
    cp -r "$ROLLBACKSCRIPTSDIR"/. "$SCRIPTSDIR"
	[ $? -ne 0 ] && { RollbackFailed=true; ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_ROLLBACKFAILED" ;}
}


#=== FUNCTION ================================================================
# NAME: RollbackFW
# DESCRIPTION: Rolls back Firmware
# PARAMETER : NA 
#=============================================================================
RollbackFW()
{
    #remove old files from slave update dir
    rm -rf $SLAVEFILEDIR/*
	mkdir -p $SLAVEFILEDIR 
	local FWCount=0
    #FIRMWARES_TO_BE_UPDATED contains all the firmwares that were getting updated. We rollback only those.
    for Name in "${FIRMWARES_TO_BE_UPDATED[@]}"; do
        local FirmwareName=$(GenerateFirmwareName $Name)
        Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE"  "$ROLLBACKFIRMWAREDIR/$FirmwareName" "$SLAVEFILEDIR/$Name" 
    	cp $ROLLBACKFIRMWAREDIR/$FirmwareName $SLAVEFILEDIR/$Name
        if [ $? -ne 0 ]; then
            RollbackFailed=true
            ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$ROLLBACKFIRMWAREDIR/" "$SLAVEFILEDIR"
        fi
		((FWCount++))
    done
	if [ $FWCount -ne 0 ];then
    	ExecutePTS "$SLAVEFILEDIR" "DontRollbackOnFailure"
		if [ $? -ne 0 ];then 
        	RollbackFailed=true
        	ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_ROLLBACKFAILED"
		else
			cp $SLAVEFILEDIR/* $FIRMWAREDIR/
		fi
    fi
}


#=== FUNCTION ================================================================
# NAME: RollbackBoardOptions 
# DESCRIPTION: Rolls back Board options. JS File present in Bin will be used
# PARAMETER : NA 
#=============================================================================
#RollbackBoardOptions()
#{
#    GenerateBoardOptionsTemp "rollbackmode"
#    ExecutePTS "$BOARDOPTIONSFILETEMP" "DontRollbackOnFailure" 
#	[ $? -ne 0 ] && { RollbackFailed=true; ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_ROLLBACKFAILED" ;}
#}

#=== FUNCTION ================================================================
# NAME: RollbackSW
# DESCRIPTION: To perform Rollback of SW on error 
# PARAMETER : NA 
#=============================================================================
RollbackSW()
{
	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_ROLLINGBACK" "$BINDIR,$SETTINGDIR,$TRANSLATIONSSERVDIR and $TRANSLATIONSDIR" "$ROLLBACKBINDIR/"
	cp -rf $ROLLBACKBINDIR $ROOTDIR/
	if [ $? -ne 0 ]; then
        RollbackFailed=true
        ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$ROLLBACKBINDIR/" "$ROOTDIR/"
    fi
	
	cp -r $ROLLBACKSETTINGSDIR $ROOTDIR
	if [ $? -ne 0 ];then
        RollbackFailed=true
        ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$ROLLBACKSETTINGSDIR" "$SETTINGDIR"
    fi

    cp -r $ROLLBACKTRANSLATIONS $ROOTDIR
	if [ $? -ne 0 ];then 
        RollbackFailed=true
        ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$ROLLBACKTRANSLATIONS" "$TRANSLATIONSDIR"
    fi

    cp -r $ROLLBACKTRANSLATIONSSERV $ROOTDIR
	if [ $? -ne 0 ];then 
        RollbackFailed=true
        ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$ROLLBACKTRANSLATIONSSERV" "$TRANSLATIONSSERVDIR"
    fi
}

#=== FUNCTION ================================================================
# NAME: Rollback 
# DESCRIPTION: Toplevel Rollback interface. Calls specific rollback function.
#              [Recursive function]
#               
# PARAMETER : NA 
#=============================================================================
Rollback()
{
    case $1 in
        "InitScripts")
            RollbackInitScripts
            ;;
        "SW")
            RollbackSW
            [ "$InitScriptsUpdated" = "true" ] && Rollback "InitScripts"
            ;;
        "FW")
            RollbackFW
            [ "$SwUpdated" = "true" ] && Rollback "SW"
            ;;
#        "BoardOptions")
#            RollbackBoardOptions
#            if [ "$FwUpdated" = "true" ]; then  
#                Rollback "FW"
#            elif [ "$SwUpdate" = "true" ]; then
#                Rollback "SW"
#            fi
#            ;;
        "All")
            RollbackInitScripts
            RollbackSW
            RollbackFW
#            RollbackBoardOptions
            ;;
    esac
}
#=== FUNCTION ================================================================
# NAME: UpdateSettingForRollback
# DESCRIPTION: To Update Settings to Rollback folder after update process is successful
# PARAMETER : NA
#=============================================================================
UpdateSettingForRollback()
{
    cd $SETTINGDIR
    for File in "${LIST_OF_FILES_FOR_UPDATE_ROLLBACK[@]}"; do
        cp -r $File $ROLLBACKSETTING
    done
    cd -
    return 0
}

#=== FUNCTION ================================================================
# NAME: UpdateRollback
# DESCRIPTION: To Update Rollback folder after update process is successful 
# PARAMETER : NA 
#=============================================================================
UpdateRollback()
{	
	Clean
    
	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$BINDIR" "$ROLLBACKDIR"
        # if any warning occurs then log into the log file so that it can be tracked.
        # If any file error occurs while copying the file cp discards the one which is not possible
        # and tries to copy other files
	cp -rf $BINDIR $ROLLBACKDIR 2>&1 
    [[ $? -ne 0 ]] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$BINDIR $ROLLBACKDIR"

	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$FIRMWAREDIR" "$ROLLBACKDIR"
	cp -rf $FIRMWAREDIR $ROLLBACKDIR
    [[ $? -ne 0 ]] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$FIRMWAREDIR" "$ROLLBACKDIR"

	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$SETTINGDIR" "$ROLLBACKDIR"
	#cp -rf $SETTINGDIR $ROLLBACKDIR
    UpdateSettingForRollback
    [[ $? -ne 0 ]] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$SETTINGDIR" "$ROLLBACKDIR"

	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$TRANSLATIONSDIR" "$ROLLBACKDIR"
	cp -rf $TRANSLATIONSDIR $ROLLBACKDIR
    [[ $? -ne 0 ]] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$TRANSLATIONSDIR" "$ROLLBACKDIR"

	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$TRANSLATIONSSERVDIR" "$ROLLBACKDIR"
	cp -rf $TRANSLATIONSSERVDIR $ROLLBACKDIR
    [[ $? -ne 0 ]] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$TRANSLATIONSSERVDIR" "$ROLLBACKDIR"

    mkdir -p $ROLLBACKSCRIPTSDIR $ROLLBACKINITSCRIPTS
    [[ $? -ne 0 ]] && ExitOnError

	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$EBOXINITSCRIPTSDIR" "$ROLLBACKDIR"
    cp -rf $EBOXINITSCRIPTSDIR/EBox-*.sh $ROLLBACKINITSCRIPTS
    [[ $? -ne 0 ]] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$EBOXINITSCRIPTSDIR" "$ROLLBACKINITSCRIPTS"
	
    Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$SCRIPTSDIR" "$ROLLBACKDIR"
    cp -rf $SCRIPTSDIR/. $ROLLBACKSCRIPTSDIR 
    [[ $? -ne 0 ]] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$SCRIPTSDIR" "$ROLLBACKSCRIPTSDIR"
    
    cd $ROLLBACKDIR 
    find . -type f ! -name .md5sum.txt -exec md5sum {} \;  | sed 's/.\///' | tee .md5sum.txt    
    cd - > /dev/null 2>&1
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
	local SWCount=0
    local Name
	for Name in "${!UpdatePkgSWNameVersionMap[@]}";do
        #chop the version string , e.g. MAIN_1.123 will become 1.123
        local  CurrentSWVersion=$(echo ${CurrSWNameVersionMap[$Name]} | cut -d_ -f2)
        local  TmpSWVersion=$(echo ${UpdatePkgSWNameVersionMap[$Name]} | cut -d_ -f2)
		local  Return=$(CompareVersion $CurrentSWVersion $TmpSWVersion)
		if [ "$Return" = "LOWER" ]; then
		    ((SWCount++))
            Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$TMPBINDIR/$Name" "$BINDIR/$Name"
            Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_UPDATING_FILE" "$Name" "${CurrSWNameVersionMap[$Name]}" "${UpdatePkgSWNameVersionMap[$Name]}"
				
            cp -f $TMPBINDIR/$Name $BINDIR/$Name  &>/dev/null
			[ $? -ne 0 ] && LogRollBackAndExit "SW" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$TMPBINDIR/$Name" "$BINDIR/$Name" 
            UPDATED_SW_BINARIES+=( "$Name" )
            xmlstarlet ed -L -u "//file[@Filename='$Name']/@Version" -v ${UpdatePkgSWNameVersionMap[$Name]} $SWVERFILE
	    fi
	done
    if [ ${#UPDATED_SW_BINARIES[@]} -gt 0 ]; then 
        UpdateSettingsAndTranslations	
        UpdateMd5SumForBin
        UpdateMd5SumForSettings
        SwUpdated=true;
    fi
}

#=== FUNCTION ================================================================
# NAME: CreateDirAndFileFilterListForSettings
# DESCRIPTION: Creates file filter options for find command, which is used
#              to copy only required files to "Settings". For example, 
#              instrument specific files are not copied.
# PARAMETER : NA 
#=============================================================================
CreateDirAndFileFilterListForSettings()
{
    local  BaseString="-name"
    local Limit=${#READONLY_OR_INSTRUMENT_SPECIFIC_DIR_LIST[@]}
    for (( i = 0; i < Limit; i++))
    do
        if  (( i < Limit - 1 )) ; then
            DIR_FILTER_LIST+=("$BaseString "${READONLY_OR_INSTRUMENT_SPECIFIC_DIR_LIST[$i]}" -o ")
        else
            DIR_FILTER_LIST+=("$BaseString "${READONLY_OR_INSTRUMENT_SPECIFIC_DIR_LIST[$i]}"")
        fi
    done

    Limit=${#READONLY_OR_INSTRUMENT_SPECIFIC_FILES[@]}
    for (( i = 0; i < Limit; i++))
    do
        if  (( i < Limit - 1 )) ; then
            FILE_FILTER_LIST+=("$BaseString "${READONLY_OR_INSTRUMENT_SPECIFIC_FILES[$i]}" -o ")
        else
            FILE_FILTER_LIST+=("$BaseString "${READONLY_OR_INSTRUMENT_SPECIFIC_FILES[$i]}"")
        fi
    done
}

#=== FUNCTION ================================================================
# NAME: RemoveObsoleteFiles
# DESCRIPTION: Removes obsolete files present in Settings
# PARAMETER : NA 
#=============================================================================
RemoveObsoleteFiles()
{
    cd $SETTINGDIR 
    # find . -type f | while read FileName; do
    #     if [ ! -f $TMPSETTINGDIR/$FileName ]; then 
    #         Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_REMOVE_OBSOLETE_FILE" "$SETTINGDIR"/"$FileName"
    #         rm $FileName
    #         local BaseFileName=${FileName##*/}
    #         #remove entry from md5sum file
    #         sed -i "/$BaseFileName/d" .md5sum.txt
    #     fi
    # done
    cd - >/dev/null 2>&1
}


#=== FUNCTION ================================================================
# NAME: UpdateSettingsAndTranslations
# DESCRIPTION: Updates Settings and Translations to the ones present in 
#              update package. 
#              NOTE: Instrument specific files/ Readonly files are not copied
#              for now.
# PARAMETER : NA 
#=============================================================================
UpdateSettingsAndTranslations()
{
    CreateDirAndFileFilterListForSettings
    
    cd $TMPSETTINGDIR
    #copy only required files & folder to target Settings
    local FILES_FOLDERS_TO_COPY=($(find . \( ${FILE_FILTER_LIST[@]} -o ${DIR_FILTER_LIST[@]} \) -prune -o -print | sed 's/\.$//'))  
    for File in "${FILES_FOLDERS_TO_COPY[@]}";do
        cp -r "$File" $SETTINGDIR
    done
    RemoveObsoleteFiles

    #update translations    
    cp -r $TMPTRANSLATIONDIR/. $TRANSLATIONSDIR
    cp -r $TMPTRANSLATIONSSERVICEDIR/. $TRANSLATIONSSERVDIR
}


#=== FUNCTION ================================================================
# NAME: UpdateInitScripts
# DESCRIPTION: update init scripts
# PARAMETER : NA 
#=============================================================================
UpdateInitScripts()
{
    local Name
	for Name in "${!UpdatePkgInitScriptsNameVersionMap[@]}";do
    	Return=$(CompareVersion ${CurrInitScriptsNameVersionMap[$Name]} ${UpdatePkgInitScriptsNameVersionMap[$Name]})
	    if [ "$Return" = "LOWER" ]; then
            Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_UPDATING_FILE" "$Name" "${CurrInitScriptsNameVersionMap[$Name]}" "${UpdatePkgInitScriptsNameVersionMap[$Name]}"
            cp $TMPINITD/* $EBOXINITSCRIPTSDIR
            cp -r $TMPSCRIPTS/. $SCRIPTSDIR
            xmlstarlet ed -L -u "//file[@Filename='$Name']/@Version" -v ${UpdatePkgInitScriptsNameVersionMap[$Name]} $SWVERFILE
            InitScriptsUpdated="true"
        fi
    done
    if [ $InitScriptsUpdated = "true" ]; then
        UpdateMd5SumForSWVersionXml
        VerifyMd5sum $SETTINGDIR
        [ $? -ne 0 ] && LogRollBackAndExit "InitScripts" "$EVENT_SWUPDATE_ERROR_MD5SUM" "$SETTINGDIR"
    fi
}


#=== FUNCTION ================================================================
# NAME: UpdateBoardOptions
# DESCRIPTION: Interface to update Slave Board options
# PARAMETER : NA 
#=============================================================================
#UpdateBoardOptions()
#{
#   local Name
#	for Name in "${!UpdatePkgBoardOptionNameVersionMap[@]}";do
#    	local  CompareResult=$(CompareVersion ${CurrBoardOptionNameVersionMap[$Name]} ${UpdatePkgBoardOptionNameVersionMap[$Name]})
#	    if [ "$CompareResult" = "LOWER" ]; then
#            Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_UPDATING_FILE" "$Name" "${CurrBoardOptionNameVersionMap[$Name]}" "${UpdatePkgBoardOptionNameVersionMap[$Name]}"
#            GenerateBoardOptionsTemp
#            ExecutePTS "$BOARDOPTIONSFILETEMP" 
#            xmlstarlet ed -L -u "//file[@Filename='$Name']/@Version" -v ${UpdatePkgBoardOptionNameVersionMap[$Name]} $SWVERFILE
#            BoardOptionsUpdated="true"
#        fi
#    done
#    if [ "$BoardOptionsUpdated" = "true" ]; then 
#        cp $BOARDOPTIONSFILE $BINDIR
#        local BoardOptionsFileName=$(basename $BOARDOPTIONSFILE)
#        local BoardOptionsFileMd5Sum=$(grep "\sBoardOptionsFileName$" $TMPBINDIR/.md5sum.txt)
#        sed -i "/\s$BoardOptionsFileName$/c\\$BoardOptionsFileMd5Sum" $BINDIR/.md5sum.txt
#        VerifyMd5sum $BINDIR
#        [ $? -ne 0 ] && LogRollBackAndExit "BoardOptions" "$EVENT_SWUPDATE_ERROR_MD5SUM" "$BINDIR"
 #       UpdateMd5SumForSWVersionXml
 #       VerifyMd5sum $SETTINGDIR
 #       [ $? -ne 0 ] && LogRollBackAndExit "BoardOptions" "$EVENT_SWUPDATE_ERROR_MD5SUM" "$SETTINGDIR"
#    fi
#}


#=== FUNCTION ================================================================
# NAME: StoreDataReadFromPackageSWVersionXml
# DESCRIPTION: Stores data read from SW_Version.xml in update pacakge to 
#              arrays
# PARAMETER : NA 
#=============================================================================
StoreDataReadFromPackageSWVersionXml()
{
    UpdatePkgSWNames=(""$SWNAME"")
    UpdatePkgSWVersions=(""$SWVER"")
    CheckSWVersionString
    UpdatePkgFWNames=(""$FWNAME"")	
    UpdatePkgFWVersions=(""$FWVER"")
    CheckVersionString UpdatePkgFWVersions[@]
    UpdatePkgInitScriptName=(""$INITSCRIPTNAME"")	
    UpdatePkgInitScriptVer=(""$INITSCRIPTVERSION"")
    CheckVersionString UpdatePkgInitScriptVer[@]
 #   UpdatePkgBoardOptionName=(""$BOARDOPTIONNAME"")	
 #  UpdatePkgBoardOptionVer=(""$BOARDOPTIONVERSION"")
 # CheckVersionString UpdatePkgBoardOptionVer[@]
    
	local Index=0
    local Name
	for Name in "${UpdatePkgSWNames[@]}";do
		[ "$Name" == "" ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED" "$UPDATESWVERFILE"
		UpdatePkgSWNameVersionMap[$Name]=${UpdatePkgSWVersions[$Index]}
		((Index++))
	done

	Index=0
	# store the data in associative array using the key value(FW name)
	for Name in "${UpdatePkgFWNames[@]}";do
		[ "$Name" == "" ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED" "$UPDATESWVERFILE"
		UpdatePkgFWNameVersionMap[$Name]=${UpdatePkgFWVersions[$Index]}
		((Index++))
	done

	Index=0
	# store the data in associative array using the key value(FW name)
	for Name in "${UpdatePkgInitScriptName[@]}";do
		[ "$Name" == "" ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED" "$UPDATESWVERFILE"
		UpdatePkgInitScriptsNameVersionMap[$Name]=${UpdatePkgInitScriptVer[$Index]}
		((Index++))
	done
    
	Index=0
	# store the data in associative array using the key value(FW name)
#	for Name in "${UpdatePkgBoardOptionName[@]}";do
#		[ "$Name" == "" ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED" "$UPDATESWVERFILE"
#		UpdatePkgBoardOptionNameVersionMap[$Name]=${UpdatePkgBoardOptionVer[$Index]}
#		((Index++))
#	done
}

#=== FUNCTION ================================================================
# NAME: CheckSWVersionString
# DESCRIPTION: Checks if the version strings present in the passed array
#              are valid. 
# PARAMETER : Array containing version strings
#=============================================================================
CheckSWVersionString() 
{
   for Version in "${UpdatePkgSWVersions[@]}";do
       Version=$(echo $Version | cut -d_ -f2)
       #check if version consists 1-3 digits before the decimal point and
       # 1-3 digits after the decimal point.
       echo "$Version" | grep "\b[0-9]\{1,3\}\.[0-9]\{1,3\}$"
       [ $? -ne 0 ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED" "$UPDATESWVERFILE"
   done
}

#=== FUNCTION ================================================================
# NAME: CheckVersionString
# DESCRIPTION: Checks if passed version value is a number in range 0-999
# PARAMETER : Array containing version numbers
#=============================================================================
CheckVersionString()
{
  for Version in "${!1}";do
       echo "$Version" | grep "\b[0-9]\{1,3\}$" #begins with a number,digits min 1, max 3
       [ $? -ne 0 ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED" "$UPDATESWVERFILE"
  done
  return 0
}

#=== FUNCTION ================================================================
# NAME: StoreDataReadFromExistingSWVersionXml
# DESCRIPTION: Stores data read from SW_Version.xml in Settings to 
#              arrays
# PARAMETER : NA 
#=============================================================================
StoreDataReadFromExistingSWVersionXml()
{
	# converting the string to array
	CurrSWNames=(""$SWNAME"")	
    CurrSWVer=(""$SWVER"")
	CurrFWNames=(""$FWNAME"")	
	CurrFWVer=(""$FWVER"")	
	CurrInitScriptName=(""$INITSCRIPTNAME"")	
	CurrInitScriptVer=(""$INITSCRIPTVERSION"")	
#	CurrBoardOptionName=(""$BOARDOPTIONNAME"")	
#	CurrBoardOptionVer=(""$BOARDOPTIONVERSION"")	
	
	local Index=0
	local Name
	# store the data in associative array using the key value(SW name)
	for Name in "${CurrSWNames[@]}";do
		# check the name exists in the array as empty or not, if empty don't create array
		[ "$Name" == "" ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED" "$SWVERFILE"
		CurrSWNameVersionMap[$Name]=${CurrSWVer[$Index]}
		((Index++))
	done

	Index=0
	# store the data in associative array using the key value(FW name)
	for Name in "${CurrFWNames[@]}";do
		[ "$Name" == "" ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED" "$SWVERFILE"
		CurrFWNameVersionMap[$Name]=${CurrFWVer[$Index]}
		((Index++))
	done


	Index=0
	# store the data in associative array using the key value(FW name)
	for Name in "${CurrInitScriptName[@]}";do
		[ "$Name" == "" ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED" "$SWVERFILE"
	    CurrInitScriptsNameVersionMap[$Name]=${CurrInitScriptVer[$Index]}
		((Index++))
	done

	Index=0
	# store the data in associative array using the key value(FW name)
#	for Name in "${CurrBoardOptionName[@]}";do
#		[ "$Name" == "" ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED" "$SWVERFILE"
#		CurrBoardOptionNameVersionMap[$Name]=${CurrBoardOptionVer[$Index]}
#		((Index++))
#	done
}


#=== FUNCTION ================================================================
# NAME: CompareExistingAndPackageXmlData
# DESCRIPTION: Compare Existing and Package Version information. 
# PARAMETER : NA 
#=============================================================================
CompareExistingAndPackageXmlData()
{
    diff <(printf "%s\n" "${CurrSWNames[@]}") <(printf "%s\n" "${UpdatePkgSWNames[@]}") > /dev/null 2>&1
    [ $? -ne 0 ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_XML_NODE_ERROR"

    diff <(printf "%s\n" "${CurrFWNames[@]}") <(printf "%s\n" "${UpdatePkgFWNames[@]}") > /dev/null 2>&1
    [ $? -ne 0 ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_XML_NODE_ERROR"

    diff <(printf "%s\n" "${CurrInitScriptName[@]}") <(printf "%s\n" "${UpdatePkgInitScriptName[@]}") > /dev/null 2>&1
    [ $? -ne 0 ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_XML_NODE_ERROR"

  #  diff <(printf "%s\n" "${CurrBoardOptionName[@]}") <(printf "%s\n" "${UpdatePkgBoardOptionName[@]}") > /dev/null 2>&1
   # [ $? -ne 0 ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_XML_NODE_ERROR"
}

#=== FUNCTION ================================================================
# NAME: ReadXML 
# DESCRIPTION: Reads SW_Version.xml and stores filenames and versions in two
#              separate strings
# PARAMETER 1: SW_Version.xml ( with path )
#=============================================================================
ReadXML()
{        
    xmlstarlet val "$1" 
    [ $? -ne 0 ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_TMP_SWVERSION_FILE_CORRUPTED" "$1"

	SWNAME="$(xmlstarlet sel -t -m '(/SW_Version/Master_SW/file)' -v @Filename -n  "$1" 2>/dev/null)"

    SWVER="$(xmlstarlet sel -t -m '(/SW_Version/Master_SW/file)' -v @Version -n  "$1" 2>/dev/null)"

    FWNAME="$(xmlstarlet sel -t -m '(/SW_Version/Firmware/file)' -v @Filename -n  "$1" 2>/dev/null)"

    FWVER="$(xmlstarlet sel -t -m '(/SW_Version/Firmware/file)' -v @Version -n  "$1" 2>/dev/null)"

	INITSCRIPTNAME="$(xmlstarlet sel -t -m '(/SW_Version/InitScripts/file)' -v @Filename -n  "$1" 2>/dev/null)"

    INITSCRIPTVERSION="$(xmlstarlet sel -t -m '(/SW_Version/InitScripts/file)' -v @Version -n  "$1" 2>/dev/null)"

 #   BOARDOPTIONNAME="$(xmlstarlet sel -t -m '(/SW_Version/BoardOptions/file)' -v @Filename -n  "$1" 2>/dev/null)"

 #   BOARDOPTIONVERSION="$(xmlstarlet sel -t -m '(/SW_Version/BoardOptions/file)' -v @Version -n  "$1" 2>/dev/null)"
}

#=== FUNCTION ================================================================
# NAME: ProcessSWVersionXml
# DESCRIPTION: Top level interface for processing SW version xml present in
#              Settings and update package  
# PARAMETER : NA 
#=============================================================================
ProcessSWVersionXml()
{
    [ ! -f $UPDATESWVERFILE ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS" "$UPDATESWVERFILE"
    [ ! -f $SWVERFILE ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS" "$SWVERFILE"

	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_PARSING_XML" "$UPDATESWVERFILE"
    ReadXML $UPDATESWVERFILE
    StoreDataReadFromPackageSWVersionXml

	Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_PARSING_XML" "$SWVERFILE"
	ReadXML $SWVERFILE
    StoreDataReadFromExistingSWVersionXml

    CompareExistingAndPackageXmlData
	return 0
}

#=== FUNCTION ================================================================
# NAME: MasterSWUpdate
# DESCRIPTION:  Interface for Master software update 
# PARAMETER : NA 
#=============================================================================
MasterSWUpdate()
{
	# Checking for presence of Rollback folder
	if [ -d "$ROLLBACKDIR" ]; then	
		UpdateBootConfigFile "PowerFailed" "Yes"
		UpdateBootConfigFile "Software_Update_Status" "Failure"
        ProcessSWVersionXml
        UpdateInitScripts
		UpdateSWBinaries
		UpdateSlaveFW
#        UpdateBoardOptions
        CheckIfAnyModulesWereUpdated
        if [ $? -eq 0 ]; then
            UpdateBootConfigFile "Software_Update_Status" "Success"
        else
            UpdateBootConfigFile "Software_Update_Status" "HigherVersionNA"
        fi
		UpdateBootConfigFile "PowerFailed" "No"
    else
        ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS" "$ROLLBACKDIR"
    fi
}

#==============================================================
#  Interface for starting SW Update related procedures, which 
#  includes updating Software Binaries and configuration,Firmware,
#  InitScripts and Slave Board Options.
#  PARAMETER 1:
# -update:  	   Update the latest SW and Slave FW files
# -updateRollback: Update the rollback folder
# -rollback:       rollback all modules
#  PARAMETER 2: Base Event Id
#  Usage ex: ./MasterSWUpdate.sh -update 100
#==============================================================

[ ! $# -eq 2 ] && { ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_INVALID_CMD_ARGS" "$CMDARG"; }

case "$1" in
    -update)
        BASE_EVENT_ID=$2
        UpdateStarted=true
        #Start the power fail monitoring script
#        chmod 755  $TMPBINDIR/$POWERFAILSCRIPT
#        exec $TMPBINDIR/$POWERFAILSCRIPT & > /dev/null 2>&1
        #exec $BINDIR/ImageTestApp $SETTINGDIR/SWUpdate_Running.png -qws & >/dev/null 2>&1
        exec $BINDIR/ImageTestApp 17891338 -qws & >/dev/null 2>&1
        #turn on lcd
        lcd on
        MasterSWUpdate
        Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_SUCCESS"
#        $(kill -9 $(pidof -x PowerFailMonitor.sh)) > /dev/null 2>&1
        $(kill -9 $(pidof ImageTestApp)) > /dev/null 2>&1
        #reboot the os if init script is updated
        if [ $InitScriptsUpdated = true ]; then
            reboot
        fi 
        exit 0
        ;;
    -updateRollback)
        BASE_EVENT_ID=$2	
        UpdateRollback	
        exit 0
        ;;
    -rollback)
        BASE_EVENT_ID=$2
        #turn on lcd
        lcd on
        #$BINDIR/ImageTestApp $SETTINGDIR/SWUpdate_Rollback.png -qws & > /dev/null 2>&1
        $BINDIR/ImageTestApp 17891337 -qws & > /dev/null 2>&1
        Rollback "All"
        Clean
        exit 0
        ;;
    *)
        ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_INVALID_CMD_ARGS" "$CMDARG"	
        ;;
esac

