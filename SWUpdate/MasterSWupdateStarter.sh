#!/bin/bash

#****************************************************************************/
#! \file MasterSWupdateStarter.sh
#
#  \brief: A simple interface to kick start Software update via USB/RemoteCare.
#          Provides following functionality:
#              -Select highest version sw update package available via
#               Remote care or USB
#              -File and meta data integrity checks via md5sum and CRC
#              -Initiate Master SW update if package is valid.
#              - 
#   $Version: $ 1.1
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
#****************************************************************************/

# Extract directory path from where this script is executing
CURRDIR="$( cd "$( dirname "$0" )" && pwd )"

if [ -f $CURRDIR/Utils.sh ]; then
    . $CURRDIR/Utils.sh
else
    echo "$CURRDIR/Utils.sh file missing"
    exit 1
fi		

declare -A CurrentSlaveHWVersionMap
declare -A UpdatePkgSlaveHWVersionMap

# Extract current script name
PROGNAME=$(basename $0)	

# Command line argument		
readonly CMDARG="Usage: $0 [-check USB/RemoteCare baseeventid devicename] [-update baseeventid] \\n 
or usage: $0 [-updateRollback/-rollback/-clean baseeventid]"


#--------------------------------------------------------------------
#	Making Log folder and Log file if it doesn't exist
#       Creating header entry in LOG File if log file is empty
#--------------------------------------------------------------------

mkdir -p $LOGDIR  2> /dev/null
if [ $? -ne 0 ]; then
    echo "Creating $LOGDIR failed"	
    exit 1
fi
DATE=`date +%Y-%m-%d,%T:%N | sed 's/......$//'`
if [ ! -s $LOGDIR/$LOGFILE ]; then
    echo -n "" >> $LOGDIR/$LOGFILE
    echo -e "Format Version: 1 \nComponent: Master SW Update \nTimeStamp: $DATE \nBaseEventID: $BASE_EVENT_ID \n" >> $LOGDIR/$LOGFILE
else
    sed -i "/^TimeStamp/c\TimeStamp:$DATE" $LOGDIR/$LOGFILE
fi

#=== FUNCTION =========================================================================
# NAME: CheckIfUpdatePackageIsAvailable
# DESCRIPTION: Checks if Software update package is available from RemoteCare
# PARAMETER: NA
#======================================================================================
CheckIfUpdatePackageIsAvailable()
{
    ls $PKGPATH/"$DEVICENAME""$PKGFOOTER"*.zip &>/dev/null
    return $?
}

#=== FUNCTION =========================================================================
# NAME: SelectPackage
# DESCRIPTION: If more than one SW Update package is found, select the package with
#              highest version number.
# PARAMETER: NA
#======================================================================================
SelectPackage()
{

    cd $PKGPATH                                                                        
    local Pkglist=($(ls "$DEVICENAME""$PKGFOOTER"*.zip))                               
    for Package in "${Pkglist[@]}";do                                                  
       local DateExtnString=$(echo $Package| awk -F_ '{print $6}')                     
       [ ${#DateExtnString} -gt 12 ] && continue                               
       PkglistFiltered+=( "$Package" )                                                             
    done     

    for Package in "${PkglistFiltered[@]}"; do                    
        Versionlist+=( $(echo $Package | cut -d'_' -f5) )             
    done                                                             
    
    local Max=0                                               
    for Version in "${Versionlist[@]}";do                            
        local Ret=$(CompareVersion $Version $Max)        
        [ "$Ret" = "GREATER" ] && Max=$Version   
    done                                         
    local MaxVersion=$Max                                 
    local Pkg=($(ls "$DEVICENAME""$PKGFOOTER""$Max"*.zip))
    for DatePkg in "${Pkg[@]}";do                                            
        local DateString=$(echo "$DatePkg"| awk -F_ '{print $6}')            
        DateList+=( "${DateString%.*}" )                         
    done                                                                                                                             
    Max=0                                                                     
    for Date in "${DateList[@]}";do                                                    
        local Ret=$(CompareVersion $Date $Max)                   
        [ "$Ret" = "GREATER" ] && Max=$Date   
    done                                                                               
    local MaxDate=$Max
    PkgName="$DEVICENAME""$PKGFOOTER""$MaxVersion"_"$MaxDate".zip
    echo $PkgName                                         
    cd - >/dev/null 2>&1 
}


#=== FUNCTION =========================================================================
# NAME: CopyPackageToUpdateDirectory
# DESCRIPTION: Copies update package to temporary directory for further processing.
# PARAMETER: NA
#======================================================================================
CopyPackageToUpdateDirectory()
{
    mkdir -p $UPDATEDIR || ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_CREATION_FAILED" "$UPDATEDIR"
    Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_COPYING_FILE" "$PKGPATH/$PkgName" "$UPDATEDIR"
    cp $PKGPATH/$PkgName $UPDATEDIR/
    [ $? -ne 0 ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_COPY_FAILED" "$PKGPATH/$PkgName" "$UPDATEDIR"
}

#=== FUNCTION =========================================================================
# NAME: CheckCRCOfSWUpdatePackage
# DESCRIPTION: Checks CRC of the zipped SW Update package
# PARAMETER: NA
#======================================================================================
CheckCRCOfSWUpdatePackage()
{
    # Check for CRC of zipped SW package
    unzip -t $UPDATEDIR/$PkgName &>/dev/null
    [ $? -ne 0 ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_CRC_CHK_FAILED"
    Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_CRC_CHK_PASSED"
}


#=== FUNCTION =========================================================================
# NAME: UnzipSWUpdatePackage
# DESCRIPTION: Extracts the SW Update package
# PARAMETER: NA
#======================================================================================
UnzipSWUpdatePackage()
{
    # Unzip SW Update Package
    unzip -d $UPDATEPKGDIR $UPDATEDIR/$PkgName &>/dev/null
    [ -d $TMPBINDIR ] && [ -d $TMPFIRMWAREDIR ] && [ -d $TMPSETTINGDIR ] && [ -d $TMPTRANSLATIONDIR ] \
        && [ -f $UPDATESWVERFILE ] && [ -d $TMPINITD ] && [ -d $TMPSCRIPTS ] && [ -d $TMPTRANSLATIONSSERVICEDIR ] || ExitOnError "$EVENT_SOURCE_MASTER"\
        "$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS"\
        "$TMPBINDIR, $TMPFIRMWAREDIR, $TMPSETTINGDIR and $UPDATESWVERFILE"

}

#=== FUNCTION =========================================================================
# NAME: CheckMd5Sum
# DESCRIPTION:  Check Md5sum of the files in extracted SW Update package
# PARAMETER: NA
#======================================================================================
CheckMd5Sum()
{
    cd $UPDATEPKGDIR

    [ ! -f $MD5FILE ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS" "$MD5FILE"

    find ${LIST_OF_DIR_FOR_MD5SUM_CHECK[@]} -type f -print | xargs md5sum >> $TMPDIRMD5FILE

    md5sum -c $MD5FILE &>/dev/null
    [ $? -ne 0 ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_MD5SUM_CHK_FAILED"

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
# NAME: CheckIfSlaveHWNameIsPresentInUpdatePkg
# DESCRIPTION: Checks if current slave name is present in Slave_HW_Version.txt of 
# Update package. 
# PARAMETER: NA
# Return: 0 if present , else 1 
#======================================================================================
CheckIfSlaveHWNameIsPresentInUpdatePkg()                                              
{                                                                                      
    for PkgSlaveHWName in "${!UpdatePkgSlaveHWVersionMap[@]}";do                                                                                    
        if [ $1 = $PkgSlaveHWName ];then                                       
            return 0                                                        
        else                                                                                                  
            continue                                                                             
        fi                                                                     
    done                                                                                                                         
    return 1                                                                                                        
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

    while read ReadLine; do
        local SlaveName=$(echo $ReadLine | awk -F, '{print $1}')
        local SlaveVersion=$(echo $ReadLine | awk -F, '{print $2}')
        CurrentSlaveHWVersionMap[$SlaveName]=$SlaveVersion
    done < $HWVERFILE

    while read ReadLine; do
        local SlaveName=$(echo $ReadLine | awk -F, '{print $1}')
        local SlaveVersion=$(echo $ReadLine | awk -F, '{print $2}')
        UpdatePkgSlaveHWVersionMap[$SlaveName]=$SlaveVersion
    done < $UPDATEHWVERFILE

    for Name in "${!CurrentSlaveHWVersionMap[@]}"; do
        CheckIfSlaveHWNameIsPresentInUpdatePkg $Name
        [ $? -ne 0 ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_MISSING_SLAVE" "$Name" "$UPDATEHWVERFILE"
        local CurrentHWVersion=${CurrentSlaveHWVersionMap[$Name]}
        local RequiredHWVersion=${UpdatePkgSlaveHWVersionMap[$Name]}
        local Return=$(CompareVersion $CurrentHWVersion $RequiredHWVersion )
        if [ $Return != "EQUAL" ]; then
            ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_SLAVE_DOESNOT_HAVE_REQ_HW_VER" "$Name" "$CurrentHWVersion" "$RequiredHWVersion"		
        else
            Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_SLAVE_HAS_REQ_HW_VER" "$Name" "$CurrentHWVersion"
        fi
    done
}



#=== FUNCTION =========================================================================
# NAME: ProcessSWUpdatePackage
# DESCRIPTION: Top level interface for processing SWUpdate Package
# PARAMETER: NA
#======================================================================================
ProcessSWUpdatePackage()
{
    SelectPackage
    CopyPackageToUpdateDirectory
    CheckCRCOfSWUpdatePackage
    UnzipSWUpdatePackage
    CheckMd5Sum
    CheckSlaveHWVersion
}

#=== FUNCTION =========================================================================
# NAME: CheckUpdatePackage
# DESCRIPTION: Checks and processes software update package
# PARAMETER $1: "USB" / "RemoteCare"
#======================================================================================
CheckUpdatePackage()
{
    Clean
    if [ $1 = "USB" ]; then
        /home/Leica/Scripts/EBox-Utils.sh mount_usb search "$DEVICENAME$PKGFOOTER*.zip" > /dev/null 2>&1
        if [ $? -ne 0 ]; then
            ExitOnError  "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_MOUNTING_DEV_FAILED"
        else
            USBMounted=true
            Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_MOUNTING_DEV_PASSED" "$MOUNTPOINT" 
        fi            
        PKGPATH=$MOUNTPOINT
        ProcessSWUpdatePackage
        /home/Leica/Scripts/EBox-Utils.sh  umount_usb > /dev/null 2>&1
        USBMounted=false
    elif [ $1 = "RemoteCare" ]; then
        # Path where package is available
        PKGPATH=$REMOTECAREPATH
        CheckIfUpdatePackageIsAvailable
        if [ $? -eq 0 ]; then 
            ProcessSWUpdatePackage 
        else
            ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_UPDATE_PACKAGE_NOT_FOUND" "$PKGPATH"
        fi
    else 
        ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_INVALID_CMD_ARGS" "$CMDARG"
    fi
}

#=== FUNCTION =========================================================================
# NAME: StartUpdateScript
# DESCRIPTION: Calls the actual SWUpdate Script
# PARAMETER $1: "-update"/ "-updateRollback"
# PARAMETER $2: BASE_EVENT_ID
# RETURN : 0 on success, anything else indicate error
#======================================================================================
StartUpdateScript()
{
    if [ ! -f $TMPBINDIR/$SWUPDATESCRIPT ]; then
        ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_FILE_FOLDER_DOESNOT_EXISTS" "$TMPBINDIR/$SWUPDATESCRIPT"
    else
        Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_STARTER_SUCCESS"
        Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_EXECUTING_COMPONENT" "$TMPBINDIR/$SWUPDATESCRIPT"
        # IN1356 fix
        # Change script file permissions		
        chmod 755 $TMPBINDIR/$SWUPDATESCRIPT
        $TMPBINDIR/$SWUPDATESCRIPT $1 $2 >/dev/null 2>&1
        return $?
    fi
}

#==============================================================
# Script Begins from Here
# -check:   Check for the SW package in USB or Remote care
# -update:  Execute the MasterSWUpdate script to update the SW
#	    and Slave FW Files
# -updateRollback:Execute the MasterSWUpdate script to update the 
#		  Rollback folder. 
# -rollback: Execute MasterSWUpdate script to rollback all
#            modules
# -clean: clean temporary files
#==============================================================

case "$1" in 
    -check)
        [ $# -ne 4 ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_INVALID_CMD_ARGS" "$CMDARG"
        BASE_EVENT_ID=$3
        readonly DEVICENAME=$4
        sed -i "/^BaseEventID/c\BaseEventID:$3" $LOGDIR/$LOGFILE
        CheckUpdatePackage "$2"
        Log "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_CHECK_SUCCESS"
        exit 0
        ;;
    -update)
        [ $# -ne 2 ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_INVALID_CMD_ARGS" "$CMDARG"
        BASE_EVENT_ID=$2
        StartUpdateScript $1 $BASE_EVENT_ID
        exit $?
        ;;
    -updateRollback | -rollback)
        [ $# -ne 2 ] && ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_INVALID_CMD_ARGS" "$CMDARG"
        if [ "$2" = "defaultBaseEventID" ]; then
            BASE_EVENT_ID=$(grep -r "^BaseEventID:[0-9a-fA-f]" $LOGDIR/$LOGFILE | cut -d: -f2)
        else
            BASE_EVENT_ID=$2
        fi
        StartUpdateScript $1 $BASE_EVENT_ID        
        exit $?
        ;;
    -clean)
        Clean
        exit 0
        ;;
    *)
        ExitOnError "$EVENT_SOURCE_MASTER" "$EVENT_SWUPDATE_INVALID_CMD_ARGS" "$CMDARG"
        ;;
esac
exit 1



