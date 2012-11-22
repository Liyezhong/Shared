#!/bin/sh

#****************************************************************************/
 #! \file MasterSWupdateStarter.sh
 #
 #  \brief Master SW Update Starter Implimentation.
 #
 #   $Version: $ 0.1
 #   $Date:    $ 2012-09-14
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



#----------------------------------------------------------------------
#       Function for exit due to fatal program error
#       
#       string containing descriptive error message
#----------------------------------------------------------------------

PROGNAME=$(basename $0)

ErrorExit()
{
        echo -n $(date) 
        echo $1 >> ../Log/$LOGFILE
        echo " ${PROGNAME}: ${1:-"Unknown Error"}"  
        exit 1
}


#----------------------------------------------------------------------
#       Function to check if SW Package provided via USB
#       
#       string containing descriptive message
#----------------------------------------------------------------------

CheckUSB()
{
	mount /dev/sda1 /mnt
        if [ $? -eq 0 ]; then
                echo "Mount at sda1"
                UPDATEPATH="/mnt"
                PackageCount=$(ls $UPDATEPATH/"$DEVICENAME"_SW_Update_Package_*.zip | wc -l)
                [ $PackageCount -gt 0 ] && echo "File present at sda1" >> ../Log/$LOGFILE
                if [ $PackageCount -eq 0 ]; then
                        umount /mnt
                        mount /dev/sdb1 /mnt
                        if [ $? -eq 0 ]; then
                                echo "Mount at sdb1" && UPDATEPATH="/mnt"
                                PackageCount=$(ls $UPDATEPATH/"$DEVICENAME"_SW_Update_Package_*.zip | wc -l)
                                if [ $PackageCount -gt 0 ]; then
                                        echo "File present at sdb1" >> ../Log/$LOGFILE
                                else
                                        echo "File not present in USB" >> ../Log/$LOGFILE
                                        ErrorExit "Package not present at USB."
				fi
                        else
                                ErrorExit "Mount failed at sda2"
                        fi      
                fi
        else
                echo "Mount failed at sda1"
		umount /mnt
                mount /dev/sdb1 /mnt
                if [ $? -eq 0 ]; then
                	echo "Mount at sdb1" && UPDATEPATH="/mnt"
                        PackageCount=$(ls $UPDATEPATH/"$DEVICENAME"_SW_Update_Package_*.zip | wc -l)
                        if [ $PackageCount -gt 0 ]; then
                        	echo "File present at sdb" >> ../Log/$LOGFILE
                        else
                                echo "File not present in USB" >> ../Log/$LOGFILE
                                ErrorExit "Package not present at USB."
			fi
                fi
        fi
}

#----------------------------------------------------------------------
#       Function to check if SW Package provided via RemoteCare
#       
#       string containing descriptive message
#----------------------------------------------------------------------

CheckRemoteCare()
{
        UPDATEPATH="/sd/RemoteCare"

        ls $UPDATEPATH/"$DEVICENAME"_SW_Update_Package_*.zip || [ $? -ne 0 ] &&\
                                         ErrorExit "RemoteCare file pkg error"
        PackageCount=$(ls $UPDATEPATH/"$DEVICENAME"_SW_Update_Package_*.zip | wc -l)
        if [ $PackageCount -gt 0 ]
        then
                 echo "File present at RemoteCare" >> ../Log/$LOGFILE
                 echo" File presence at RemoteCare"
        else
                 echo "File not present in RemoteCare" >> ../Log/$LOGFILE
                 ErrorExit "File present at RemoteCare"
       fi
}


#----------------------------------------------------------------------
#       Function to unzip and check CRC status of package
#       
#       string containing descriptive message
#----------------------------------------------------------------------

UnzipCRCcheck()
{
	rm -rf ../tmp/MasterSWUpdate

        PackageVersion=$(ls $UPDATEPATH/"$DEVICENAME"_SW_Update_Package_*.zip\
                            | cut -d'_' -f5 | sort | tail -1)
        echo "Highest version of update package $PackageVersion"

        PackageName="$DEVICENAME"_SW_Update_Package_"$PackageVersion"_*.zip
        echo "Package used for updating: ${PackageName}" >> ../Log/$LOGFILE

        mkdir -p ../tmp/MasterSWUpdate || ErrorExit "Error while mking directory"
        cp $UPDATEPATH/$PackageName ../tmp/MasterSWUpdate/ || ErrorExit "Error while copying"

        unzip -t ../tmp/MasterSWUpdate/$PackageName >> ../Log/$LOGFILE
        if [ $? -eq 0 ]
        then
		echo "CRC check: Passed"
                echo "CRC pass" >> ../Log/$LOGFILE
                unzip -d ../tmp/MasterSWUpdate/UpdatePackage ../tmp/MasterSWUpdate/$PackageName >> ../Log/$LOGFILE
		cp -r ../tmp/MasterSWUpdate/UpdatePackage/*/* ../tmp/MasterSWUpdate/
		rm -r ../tmp/MasterSWUpdate/UpdatePackage/*
 		mv  ../tmp/MasterSWUpdate/bin ../tmp/MasterSWUpdate/UpdatePackage/bin
		mv  ../tmp/MasterSWUpdate/settings ../tmp/MasterSWUpdate/UpdatePackage/settings
		mv  ../tmp/MasterSWUpdate/Firmware ../tmp/MasterSWUpdate/UpdatePackage/Firmware
		 
	else
                echo "CRC Fail" >> ../Log/$LOGFILE
                ErrorExit "CRC check falied"
        fi

	umount /mnt
}



#------------------------------------------------------------------
#       Get device name from current device file system
#       Variable: "DEVICENAME" stores name of the device
#---------------------------------------------------------------------

CURRENTPATH=../
NAME=$(head -1  $CURRENTPATH/settings/Slave_HW_Version.txt)
DEVICENAME=${NAME%?}
echo "Device is $DEVICENAME"


#--------------------------------------------------------------------
#       Create log file "<Devicename>_SW_Update_Events.log"
#       Variable: "LOGFILE" stores name of log file
#--------------------------------------------------------------------

mkdir -p ../Log
echo "Device is "$DEVICENAME > ../Log/$DEVICENAME"_SW_Update_Events.log"
LOGFILE=$DEVICENAME"_SW_Update_Events.log"


date >> ../Log/$LOGFILE  

[ $# -lt 2 ] && { echo "Usage: $0 filename check USB/RemoteCare"; exit 999; }

PackageCount=0
UPDATEPATH="../../"
if [ $1 = "-check" ]
then
        if [ $2 = "USB" ]
        then
		
                CheckUSB
                echo "SW Update package present at USB"
                UnzipCRCcheck
        elif [ $2 = "RemoteCare" ]
        then
		
                CheckRemoteCare
		echo "SW Update package present at RemoteCare"
		UnzipCRCcheck
        fi
	
	exec sh ../tmp/MasterSWUpdate/UpdatePackage/bin/MasterSWUpdate.sh -check 
fi



