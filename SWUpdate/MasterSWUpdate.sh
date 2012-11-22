#!/bin/sh

#/****************************************************************************/
#/*! \file MasterSWUpdate.sh
 #
 #  \brief Master SW Update Implimentation.
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
#/****************************************************************************/



#----------------------------------------------------------------------
#	Function for exit due to fatal program error
#	
#	string containing descriptive error message
#----------------------------------------------------------------------

PROGNAME=$(basename $0)

ErrorExit()
{
	echo -n $(date) 
	echo $1 >> ../Log/$LOGFILE
        echo " ${PROGNAME}: ${1:-"Unknown Error"}"  
	echo "SWUpdate=0" > ../StartUp/SWUpdate.txt
	rm -rf FWTemp.txt SWTemp.txt *SW_Version.txt temp_SW_Version.* *FW_Version.txt 
	exit 1
}


#----------------------------------------------------------------------
#       Function to compare two versions which are real number
#       Since direct comparision of floating number is not possible in shell script
#       
#       string containing descriptive message
#----------------------------------------------------------------------
VersionCheck()
{
	CurrentInt="$(echo $1 | cut -d'.' -f1)"
	UpdateInt="$(echo $2 | cut -d'.' -f1)"
	CurrentDecimal="$(echo $1 | cut -d'.' -f2)"
	UpdateDecimal="$(echo $2 | cut -d'.' -f2)"

	if [ $CurrentInt -gt $UpdateInt ]
	then
        	echo "False" #current version greater
	elif [ $CurrentInt -eq $UpdateInt ]
	then
        	if [ $CurrentDecimal -gt $UpdateDecimal ]
		then
               		echo "False" #curr dec version grater"
        	elif [ $CurrentDecimal -eq $UpdateDecimal ]
		then
                	echo "Equal" #both are equal"
        	else
                	echo "True" #update greater"
        	fi
	else
        	echo "True" #update version is greater"
	fi
}


#----------------------------------------------------------------------
#       Function to check Md5Sum the SW Package provided 
#       
#       string containing descriptive message
#----------------------------------------------------------------------

Md5Check()
{
#	ls -R tmp/MasterSWUpdate/UpdatePackage/ >FileNames.txt
#	md5sum FileNames.txt > md5sum.txt
        md5sum -c ../tmp/MasterSWUpdate/UpdatePackage/settings/md5sum.txt >> ../Log/$LOGFILE
        if [ $? -eq 0 ]
        then
	       	echo "Md5sum: integrity of filename pass" >> ../Log/$LOGFILE
		echo "Md5SumCheck: Passed"			
        else
               	echo "Md5sum fail" >> ../Log/$LOGFILE
               	ErrorExit "Md5sum failed"
        fi
	        
}

#----------------------------------------------------------------------
#       Function to check the Slave device HW Version 
#       
#       string containing descriptive message
#----------------------------------------------------------------------

SlaveHWVersionCheck()
{
	echo "Check if Slave devices have correct HW Version"
	for CurrentSlaveName in $(tail -n+2 $CURRENTPATH/settings/Slave_HW_Version.txt | cut -d',' -f1 )
	do
		for UpdateSlaveName in $(tail -n+2 ../tmp/MasterSWUpdate/UpdatePackage/settings/Slave_HW_Version.txt\
                                         | cut -d',' -f1)
                do
			if [ "$CurrentSlaveName" = "$UpdateSlaveName" ]; then
				CurrentHWVersion=$(grep $UpdateSlaveName\
                                                   $CURRENTPATH/settings/Slave_HW_Version.txt | cut -d',' -f2)
                                UpdateHWVersion=$(grep $UpdateSlaveName\
                                                  ../tmp/MasterSWUpdate/UpdatePackage/settings/Slave_HW_Version.txt|\
                                                  cut -d',' -f2)
                                Status=$(VersionCheck $CurrentHWVersion $UpdateHWVersion)
                                if [ $Status = "True" ]; then
                                        ErrorExit "Slave not have correct version"  
				elif [ $Status = "Equal" ]; then
					echo "$UpdateSlaveName have same version"
				else
					echo "$UpdateSlaveName have correct version"
				fi
			fi
		done
	done
}

#----------------------------------------------------------------------
#       Function to update and call the Slave FW update SW 
#       
#       string containing descriptive message
#----------------------------------------------------------------------

SlaveFWupdate()
{

	sed -n '/Firmware/,/\/Firmware/p' temp_SW_Version.xml | awk -F '[<>]'\
        'BEGIN{h="Filename|Version";}{ if($2~h) printf "%s"($1 "\n"),$2}' > vtemp.txt
        sed 's/ /"/g' vtemp.txt | cut -d'"' -f3 > vtemp1.txt
        sed 's/ /"/g' vtemp.txt | cut -d'"' -f6 | cut -d"_" -f2 > vtemp2.txt
        sed 's/ /"/g' vtemp.txt | cut -d'"' -f9 > vtemp3.txt
        paste -d"," vtemp1.txt vtemp2.txt vtemp3.txt > CFW_Version.txt


	sed -n '/Firmware/,/\/Firmware/p' ../tmp/MasterSWUpdate/UpdatePackage/settings/SW_Version.xml\
        | awk -F '[<>]' 'BEGIN{h="Filename|Version";}{ if($2~h) printf "%s"($1 "\n"),$2}' > vtemp.txt
        sed 's/ /"/g' vtemp.txt | cut -d'"' -f3 > vtemp1.txt
        sed 's/ /"/g' vtemp.txt | cut -d'"' -f6 | cut -d"_" -f2 > vtemp2.txt
        sed 's/ /"/g' vtemp.txt | cut -d'"' -f9 > vtemp3.txt
        paste -d"," vtemp1.txt vtemp2.txt vtemp3.txt > UFW_Version.txt
        rm -rf vtemp*.txt

	
	FWcount=0
	cp CFW_Version.txt temp_FW_Version.txt
	
	for CurrentFWName in $(cut -d',' -f1 CFW_Version.txt )
        do
                for UpdateFWName in $(cut -d',' -f1 UFW_Version.txt)
                do
                        if [ "$CurrentFWName" = "$UpdateFWName" ]; then
				CurrentFWVersion=$(grep $CurrentFWName CFW_Version.txt |cut -d',' -f2)
		                UpdateFWVersion=$(grep $UpdateFWName UFW_Version.txt | cut -d',' -f2)
				
				Status=$(VersionCheck $CurrentFWVersion $UpdateFWVersion)
				if [ $Status = "True" ]; then
					FWcount=$((FWcount+1)) && echo "$CurrentFWName needs update"
				        sed "s/$CurrentFWName\,$CurrentFWVersion/$CurrentFWName\,$UpdateFWVersion/" \
                                        temp_FW_Version.txt > tempfile.txt
					mv tempfile.txt temp_FW_Version.txt
 					[ -f tempfile.txt ] && rm tempfile.txt
					echo $CurrentFWName >> FWTemp.txt
					mkdir -p ../tmp/MasterSWUpdate/SlaveSWUpdateFiles || ErrorExit \
                                                                          "Error while mking directory"
                			cp ../tmp/MasterSWUpdate/UpdatePackage/Firmware/$CurrentFWName \
                                           ../tmp/MasterSWUpdate/SlaveSWUpdateFiles || ErrorExit "Error while copying"
					sed '/'$CurrentFWName'/{s/'$CurrentFWVersion'/'$UpdateFWVersion'/}'\
                                            temp_SW_Version.xml > Utemp_SW_Version.xml

                                        mv Utemp_SW_Version.xml temp_SW_Version.xml	
				fi
			fi
		done
	done
	
	if [ $FWcount -eq 0 ]; then
		echo "No higher version FW file in package to update"
                cp temp_SW_Version.xml $CURRENTPATH/settings/SW_Version.xml
		UpdateRollback
		[ -f temp_FW_Version.txt ] && rm temp_FW_Version.txt
		echo "True" #And start Main SW with success message
	else
#---------------SlaveFWUpdate ---------------------------------
		#FWUpdateStatus=Call Slave FW Update SW with parameter as tmp/MasterSWUpdate/SlaveSWUpdateFiles
	#	FWUpdateStatus=False
	#	if [ "$FWUpdateStatus" = "True" ]; then
			echo "True" #And start Main SW with success message
			mv temp_SW_Version.xml $CURRENTPATH/settings/SW_Version.xml
			UpdateRollback
	#	else
	#		echo "Slave FW update faile"
	#		RollbackFW
	#	fi
#----------------------------------
	fi
	 
#	
}

#----------------------------------------------------------------------
#       Function to Rollback to old FW files on error 
#       
#       string containing descriptive message
#----------------------------------------------------------------------

RollbackFW()
{
	echo "Performing Rollback"
	rm -r ../tmp/MasterSWUpdate/SlaveSWUpdateFiles/* || ErrorExit "deleting files from Slave folder fails"
	
	for CurrentFWName in $(head FWTemp.txt)
        do
		cp ../Rollback/Firmware/$CurrentFWName ../tmp/MasterSWUpdate/SlaveSWUpdateFiles	
	done
#	Call Slave FW Update SW with parameter as tmp/MasterSWUpdate/SlaveSWUpdateFiles
	RollbackSW	
}


#----------------------------------------------------------------------
#       Function to Rollback to old SW files on error 
#       
#       string containing descriptive message
#----------------------------------------------------------------------

RollbackSW()
{
	for SWName in $(head SWTemp.txt)
        do
                cp ../Rollback/bin/$SWName $CURRENTPATH/bin/
        done

}

#----------------------------------------------------------------------
#       Function to Update Rollback folder after update process is successful 
#       
#       string containing descriptive message
#----------------------------------------------------------------------

UpdateRollback()
{
	rm -rf FWTemp.txt SWTemp.txt *SW_Version.txt temp_SW_Version.* *FW_Version.txt
	echo "Updating Rollback Folder"
        cp -r $CURRENTPATH/bin/ ../Rollback/
        cp -r $CURRENTPATH/Firmware/ ../Rollback/
        cp -r $CURRENTPATH/settings/ ../Rollback/
}


#----------------------------------------------------------------------
#       Function to update the SW Package provided 
#       
#       string containing descriptive message
#----------------------------------------------------------------------

Update()
{
	if [ -d "../Rollback" ]
        then
        	echo "Rollback folder present" >> ../Log/$LOGFILE
                
                if [ -f $CURRENTPATH/settings/SW_Version.xml ]
                then
                        echo "File SW_Version.xml present" >> ../Log/$LOGFILE 
			echo "Updating higher version available files" >> ../Log/$LOGFILE
			UpdateSWFile
		else
                        echo "SW_Version.xml file abscent"
			echo "Updating all SW files"
			cp -r ../tmp/MasterSWUpdate/UpdatePackage/bin/ $CURRENTPATH/
			mkdir -p ../tmp/MasterSWUpdate/SlaveSWUpdateFiles || ErrorExit \
                        "Error while mking directory"
                        cp -r ../tmp/MasterSWUpdate/UpdatePackage/Firmware/* ../tmp/MasterSWUpdate/SlaveSWUpdateFiles
			[ $? -ne 0 ] && ErrorExit "Error while copying"
		##       call slave FW update SW.
			echo "True"
			[ $? -ne 0 ] && cp -r ../Rollback/bin/ $CURRENTPATH/ && \
                          cp -r ../Rollback/settings/ $CURRENTPATH/ &&\
                                ErrorExit "Updated file copying failed"
                fi
        else
		echo "Rollback folder is not present" >> ../Log/$LOGFILE
                ErrorExit "Rollback folder not present"
        fi
}

#----------------------------------------------------------------------
#       Function to update the higher version SW file provided 
#       
#       string containing descriptive message
#----------------------------------------------------------------------
UpdateSWFile()
{
	
	count=0
	cp $CURRENTPATH/settings/SW_Version.xml temp_SW_Version.xml
	sed -n '/Master_SW/,/\/Master_SW/p' temp_SW_Version.xml |\
        awk -F '[<>]' 'BEGIN{h="Filename|Version";}{ if($2~h) printf "%s"($1 "\n"),$2}' > vtemp.txt
	sed 's/ /"/g' vtemp.txt | cut -d'"' -f3 > vtemp1.txt
	sed 's/ /"/g' vtemp.txt | cut -d'"' -f6 | cut -d"_" -f2 > vtemp2.txt
	sed 's/ /"/g' vtemp.txt | cut -d'"' -f9 > vtemp3.txt
	paste -d"," vtemp1.txt vtemp2.txt vtemp3.txt > CSW_Version.txt	

	cp CSW_Version.txt temp_SW_Version.txt
	sed -n '/Master_SW/,/\/Master_SW/p' ../tmp/MasterSWUpdate/UpdatePackage/settings/SW_Version.xml\
        | awk -F '[<>]' 'BEGIN{h="Filename|Version";}{ if($2~h) printf "%s"($1 "\n"),$2}' > vtemp.txt
        sed 's/ /"/g' vtemp.txt | cut -d'"' -f3 > vtemp1.txt
        sed 's/ /"/g' vtemp.txt | cut -d'"' -f6 | cut -d"_" -f2 > vtemp2.txt
        sed 's/ /"/g' vtemp.txt | cut -d'"' -f9 > vtemp3.txt
        paste -d"," vtemp1.txt vtemp2.txt vtemp3.txt > USW_Version.txt
	rm -rf vtemp*.txt
	for CurrentFileName in $(cut -d',' -f1 CSW_Version.txt)
	do
		for UpdateFileName in $(cut -d',' -f1 USW_Version.txt) 
        	do	
			if [ "$CurrentFileName" = "$UpdateFileName" ]; then
                		CurrentVersion=$(grep $UpdateFileName CSW_Version.txt | cut -d',' -f2)
                        	UpdateVersion=$(grep $UpdateFileName USW_Version.txt | cut -d',' -f2)
                        	Status=$(VersionCheck $CurrentVersion $UpdateVersion)
                        	if [ $Status = "True" ]; then
                               		echo $CurrentFileName >> SWTemp.txt 
					count=$((count+1))
					echo "Updating $CurrentFileName"
					cp ../tmp/MasterSWUpdate/UpdatePackage/bin/$CurrentFileName $CURRENTPATH/bin/
					[ $? -ne 0 ] && cp -r ../Rollback/bin/ $CURRENTPATH/ && cp -r \
                                           ../Rollback/settings/ $CURRENTPATH/ && ErrorExit "Updated file copying failed"
					sed "s/$CurrentFileName\,$CurrentVersion/$CurrentFileName\,$UpdateVersion/"\
                                             temp_SW_Version.txt > tempfile.txt
					mv tempfile.txt temp_SW_Version.txt
					sed '/'$CurrentFileName'/{s/'$CurrentVersion'/'$UpdateVersion'/}'\
                                         temp_SW_Version.xml > Utemp_SW_Version.xml
					
					mv Utemp_SW_Version.xml temp_SW_Version.xml
					[ -f tempfile.txt ] && rm tempfile.txt 
				else
					echo "$CurrentFileName already having higher version than package file" 
				fi
               		fi
	 	done
	done
	if [ $count -eq 0 ]; then
		echo "No higher version of SW file in package to update"
		cp $CURRENTPATH/settings/SW_Version.xml temp_SW_Version.xml
		FWStatus=$(ExistSlaveFW)
		if [ "$FWStatus" = "False" ];then
			ErrorExit "No higher version of SW and FW files presnt in package"
		fi
	fi

}

#----------------------------------------------------------------------
#       Function to check if higher version FW file present in package 
#       
#       string containing descriptive message
#----------------------------------------------------------------------

ExistSlaveFW()
{

	sed -n '/Firmware/,/\/Firmware/p' temp_SW_Version.xml | awk -F '[<>]'\
        'BEGIN{h="Filename|Version";}{ if($2~h) printf "%s"($1 "\n"),$2}' > vtemp.txt
        sed 's/ /"/g' vtemp.txt | cut -d'"' -f3 > vtemp1.txt
        sed 's/ /"/g' vtemp.txt | cut -d'"' -f6 | cut -d"_" -f2 > vtemp2.txt
        sed 's/ /"/g' vtemp.txt | cut -d'"' -f9 > vtemp3.txt
        paste -d"," vtemp1.txt vtemp2.txt vtemp3.txt > CFW_Version.txt
        
	sed -n '/Firmware/,/\/Firmware/p' ../tmp/MasterSWUpdate/UpdatePackage/settings/SW_Version.xml\
        | awk -F '[<>]' 'BEGIN{h="Filename|Version";}{ if($2~h) printf "%s"($1 "\n"),$2}' > vtemp.txt
        sed 's/ /"/g' vtemp.txt | cut -d'"' -f3 > vtemp1.txt
        sed 's/ /"/g' vtemp.txt | cut -d'"' -f6 | cut -d"_" -f2 > vtemp2.txt
        sed 's/ /"/g' vtemp.txt | cut -d'"' -f9 > vtemp3.txt
        paste -d"," vtemp1.txt vtemp2.txt vtemp3.txt > UFW_Version.txt
        rm -rf vtemp*.txt
	
	FWFileCount=0
        for CurrentFWName in $(cut -d',' -f1 CFW_Version.txt)
        do
                for UpdateFWName in $(cut -d',' -f1 UFW_Version.txt)
                do
                        if [ "$CurrentFWName" = "$UpdateFWName" ]; then
                                CurrentFWVersion=$(grep $UpdateFWName CFW_Version.txt | cut -d',' -f2)
                                UpdateFWVersion=$(grep $UpdateFWName UFW_Version.txt  | cut -d',' -f2)
                                Status=$(VersionCheck $CurrentFWVersion $UpdateFWVersion)
                                if [ $Status = "True" ]; then
					FWFileCount=$((FWFileCount+1))
					break
				fi
			fi
		done
	done
	if [ $FWFileCount -ne 0 ]; then
		echo "True"
	else
		echo "False"
	fi	
}

#------------------------------------------------------------------
#	Get device name from current device file system
#	Variable: "DEVICENAME" stores name of the device
#---------------------------------------------------------------------

CURRENTPATH=../
NAME=$(head -1  $CURRENTPATH/settings/Slave_HW_Version.txt) 
DEVICENAME=${NAME%?}
#echo "$DEVICENAME"


#--------------------------------------------------------------------
#	Create log file "<Devicename>_SW_Update_Events.log"
#	Variable: "LOGFILE" stores name of log file
#--------------------------------------------------------------------

#echo "Device is "$DEVICENAME > $DEVICENAME"_SW_Update_Events.log"
LOGFILE=$DEVICENAME"_SW_Update_Events.log"

#date >> ../Log/$LOGFILE  #Date at the header of log file
#cat TempFile.txt >> ../Log/$LOGFILE 


#[ $# -eq 0 ] && { echo "Usage: $0 filename check USB/RemoteCare"; exit 999; }

PackageCount=0
UPDATEPATH="."

if [ $1 = "-update" ]
then
	echo "In update"
	Update
	if [ -f $CURRENTPATH/settings/SW_Version.xml ]; then
	 SlaveFWupdate 
	 echo "SWUpdate=0" > ../StartUp/SWUpdate.txt
	else
         cp ../tmp/MasterSWUpdate/UpdatePackage/settings/SW_Version.xml $CURRENTPATH/settings/
	 echo "SWUpdate=0" > ../StartUp/SWUpdate.txt
	fi
elif [ $1 = "-check" ]
then
	[ ! -d ../tmp ] && ErrorExit "Unzipped Temp folder not present"
	echo "SWUpdate=1" > ../StartUp/SWUpdate.txt
	Md5Check
	echo "Md5 check done"
	SlaveHWVersionCheck
	
elif [ $1 = "-updateRollback" ]
then
	cp -r $CURRENTPATH/bin/ ../Rollback/
	cp -r $CURRENTPATH/Firmware/ ../Rollback/
	cp -r $CURRENTPATH/settings/ ../Rollback/
fi

rm -rf FWTemp.txt SWTemp.txt *SW_Version.txt temp_SW_Version.* *FW_Version.txt 


exit 0

