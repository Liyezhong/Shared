/****************************************************************************/
/** @file xmlErrFiles/gentest.cpp
 *
 *  @brief program for generating unit tests for XmlChecker (negative tests)
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2.8.2010
 *  $Author:    $ R.Wobst
 *
 *  @b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 *  last modified by owner: @(#) Nov 25 2010, 20:21:50
 *
 */
/****************************************************************************/

#include "DataManagement/Test/TestXMLParser.h"
#include <iostream>
using namespace std;

/** @brief generate a clean error message for unit test design
 *
 * No test on correct arguments is done - it is a help tool only.
 * The out will be catched by a Python script.
 *
 * @param argv - argv[1] must contain the name of an XML file
 */

int main(int argc, const char *argv[])
{
    using namespace DataManagement;

    if(argc < 2) exit(1);

    try
    {
        RawEntryLst desc;
	TestXML::parseXmlDesc(desc);
        XmlChecker xpars(desc);

        ResultTree result;

        result = xpars.parseFile(QString(argv[1]));
    }
    catch(Global::Exception exc)
    {
        QString res;

        FormatString().formatString(exc, res);
        cout << qPrintable(res);
    }

    return 0;
}
