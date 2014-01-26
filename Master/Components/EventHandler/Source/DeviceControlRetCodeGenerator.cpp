
/****************************************************************************/
/*! \file EventHandler/Source/DeviceControlRetCodeGenerator.cpp
 *
 *  \brief CPP file for generating DeiceControl Return Codes (ReturnCode_t) 
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2014-23-01
 *  $Author:    $ Songtao Yu 
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2014 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <EventHandler/Include/EventScenarioErrorXMLInfo.h>
#include <QFile>
using namespace EventHandler;

int main(int argc, char* argv[])
{
	// The App takes two arguments, one is Event-Scenario-Error Map XML,
	// and the other is target header file.
	if (3 != argc)
	{
		return -1;
	}
	QString strMapFile = argv[1];

    //Initialize XML Info
    EventScenarioErrXMLInfo ESEXMLInfo(strMapFile, true);
	if (false == ESEXMLInfo.InitXMLInfo())
	{
		return -1;
	}
    QString strHeaderFile = argv[2];

    QFile qFile(strHeaderFile);
    if ( false == qFile.open(QIODevice::WriteOnly | QIODevice::Truncate) )
    {
        return -1;
    }
    QTextStream out(&qFile);
    out.setIntegerBase(16);

	// Get Event-Scenaro-Error Map list
    const QVector< QSharedPointer<ESEInfo> >& ESEInfoList = ESEXMLInfo.GetESEInfoList();
    QVector< QSharedPointer<ESEInfo> >::const_iterator iter = ESEInfoList.begin();

    //First of all, get the max length of the QString for appending blank
    int maxLen = 0;
    while (iter != ESEInfoList.end())
    {
        if (maxLen < (*iter)->GetName().length())
        {
            maxLen = (*iter)->GetName().length();
        }
        iter++;
    }

    iter = ESEInfoList.begin();
    quint32 seed = 0x0030;
    while (iter !=  ESEInfoList.end())
    {
        QString strErrName = (*iter)->GetName();
        // Append blank
        if (strErrName.length()<maxLen)
        {
            strErrName.append(QString((maxLen-strErrName.length()), ' '));
        }
        strErrName.replace(0, 5, "DCL_ERR");
        strErrName = strErrName.toUpper();
        QString number;
        number.sprintf("%04X",seed++);
        strErrName += "\t= 0x" + number;
        out<<strErrName;
        iter++;
        if (iter != ESEInfoList.end())
        {
            out<<","<<endl;
        }
        else
        {
            out<<endl;
        }
    }
    qFile.close();

	return 0;
}
