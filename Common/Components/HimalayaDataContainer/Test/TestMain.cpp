/****************************************************************************/
/*! \file TestMain.cpp
 *
 *  \brief main of Data Manager test.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-01-18
 *  $Author:    $ R. Wu
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2010 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <QTest>

#include "TestReagentGroup.h"
#include "TestReagent.h"

int main(int argc, char *argv[])
{
    TestReagentGroupList testRGL;
    TestReagentList testRL;

    QTest::qExec(&testRGL, argc, argv);
    QTest::qExec(&testRL, argc, argv);

    return 0;
}
