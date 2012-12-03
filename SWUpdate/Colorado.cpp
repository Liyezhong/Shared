/****************************************************************************/
/*! \file Colorado.cpp
 *
 *  \brief Main Master SW Implimentation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-09-14
 *   $Author:  $ Vikas Jhunjhunwala
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/


#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

int main()
{
	int CheckStatus;
	int UpdateStatus;
	char source='U';
	string STRING;
	ifstream infile;
	
	infile.open("input.txt");

	while(!infile.eof()) 
        {
	        getline(infile,STRING); 
	         
        }
	infile.close();
	

	if(STRING=="yes")
	{	
		cout << "Current Main Version: 2.0\n";
		cout << "Starting Main software\n";
	//	cout << "Update package provided via USB/RemoteCare(U/R)?\n";
	//	cin >> source;
		if(source=='U')
		{
			CheckStatus=system("sh MasterSWupdateStarter.sh -check USB");
		
		}
		else if(source=='R')
		{
			CheckStatus=system("sh MasterSWupdateStarter.sh -check RemoteCare");
		}
		else
		{
			cout << "Incorrect entry: Please enter U or R\n";
		}
	
		if(CheckStatus==0)
		{
			cout << "Check status: True\n";
			UpdateStatus=system("sh ../tmp/MasterSWUpdate/UpdatePackage/bin/MasterSWUpdate.sh -update &");
	//		if(UpdateStatus==0)
	//		{
	//			cout << "Update status: True\n";
	//			system("sh ../tmp/MasterSWUpdate/UpdatePackage/bin/MasterSWUpdate.sh -updateRollback");
	//		}
	//		else
	//		{	
	//			cout << "Update status: Failed\n";
	//		}
	
		}
		else
		{
			cout << "Check status: False\n";
		}
		}
	else
	{
		exit(0);
	}
		
	return 0;
//	cout << "CheckStatus" << CheckStatus <<"\n";
}


