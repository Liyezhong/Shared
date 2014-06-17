#!/bin/bash

#/****************************************************************************/
#/*! \file PowerFailMonitor.sh
 #
 #  \brief: 	Continuously monitors the power good signal. If Power fail is
 #              detected, ABRT signal is sent to MasterSWUpdate script.
 #              When Software update is completed, MasterSWUpdate.sh shall kill
 #              this script.
 #
 #
 #   $Version: $ 1.0
 #   $Date:    $ 2013-07-25
 #   $Author:  $ N.Kamath
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

# 51 ->GPIO2_19 of the processor (i.Mx35 EBox)
readonly PowerGoodGpio=51

while [ 1 ]; do
        #Rertrieve the value of power good signal
        PowerGood=$(cat /sys/class/gpio/gpio${PowerGoodGpio}/value)

        if [ $PowerGood -eq 0 ]; then
                #Send SIGUSR1 to MasterSWUpdate.sh
                killall -SIGUSR1 MasterSWUpdate.sh
                #update BootConfig.txt power fail section
        		UpdateBootConfigFile "PowerFailed" "Yes"
		        UpdateBootConfigFile "Software_Update_Status" "Failure" 
                sync
		        exit 0
        fi
        # Power good signal is monitored every second
        sleep 1
done
