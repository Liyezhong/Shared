#****************************************************************************/
 #! \file Startup.sh
 #
 #  \brief Startup Script Implimentation.
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



#!/bin/sh

while [ True ] 
do
	echo "SWUpdate=0" > ../StartUp/SWUpdate.txt
	
	../bin/./Colorado & 
	my_pid=$!
				
	while ps | grep "$my_pid"      
	do
		echo $my_pid is still in the ps output. Main still running.
		sleep 1
	done
	
	if [ -f ../StartUp/SWUpdate.txt ]; then
		SWUpdate=$(cut -d'=' -f2 ../StartUp/SWUpdate.txt)
		echo "SW Update $SWUpdate"

		while [ $SWUpdate -eq 1 ]
		do
			SWUpdate=$(cut -d'=' -f2 ../StartUp/SWUpdate.txt)
			echo "SW Update in progress."
			sleep 1
		done

		echo "SW Update not in progress."
		sleep 2
	else
		echo "SWUpdate.txt not present" && exit 0
	fi
done
