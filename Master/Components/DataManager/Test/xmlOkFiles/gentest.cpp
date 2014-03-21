/****************************************************************************/
/** @file xmlOkFiles/gentest.cpp
 *
 *  @brief program for generating unit tests for XmlChecker (positive tests)
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 4.8.2010
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
 *  last modified by owner: @(#) Nov 25 2010, 20:21:58
 *
 */
/****************************************************************************/

#include "DataManagement/Test/TestXMLParser.h"
#include <iostream>
using namespace std;

namespace DataManagement
{
void printResultTree(ResultIterator iter, int level=0)
{
    ResultElement elem;

    while(iter.hasNext())
    {
        elem = iter.next();
        for(int i=level<<2; i--;) cout << " ";
        cout << qPrintable(elem.first) << "\t";

        if(elem.second.userType() == ResultTree::m_userType)
        {
            cout << endl;
            ResultTree child = elem.second.value<ResultTree>();
            printResultTree(child.iterator(), level+1);
        }
        else
        {
          cout << qPrintable(elem.second.toString()) << endl;
        }
    }
}

} // end namespace DataManagement

/** @brief generate a clean error message for unit test design
 *
 * No test on correct arguments is done - it is a help tool only.
 * The out will be catched by a Python script.
 *
 * @param argv - argv[1] must contain the name of an XML file, or be
 *               a sequence of XML files
 */

int main(int argc, const char *argv[])
{
    using namespace DataManagement;

    if(argc < 2)
    {
        cout << "illegal usage: prog XMLfile" << endl;
        exit(1);
    }

    QStringList ql;

    ql = QString(argv[1]).split(" ", QString::SkipEmptyParts);

    try
    {
        RawEntryLst desc;
	TestXML::parseXmlDesc(desc);
        XmlChecker xpars(desc);

        ResultTree result;

        for(QStringList::iterator fn = ql.begin(); fn != ql.end(); ++fn)
        {
            result = xpars.parseFile(*fn);
            printResultTree(result.iterator());
        }
    }
    catch(Global::Exception exc)
    {
        QString res;

        FormatString().formatString(exc, res);
        cout << qPrintable(res);
    }

    return 0;
}
