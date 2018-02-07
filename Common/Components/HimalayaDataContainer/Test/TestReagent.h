/****************************************************************************/
/*! \file TestReagent.h
 *
 *  \brief header of reagent test.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-01-31
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

#ifndef TESTREAGENT_H
#define TESTREAGENT_H

#include <QObject>


class TestReagentList : public QObject
{
    Q_OBJECT
private slots:
    void TestReagentListConstructor(void);

    void TestReagentListFileVersion(void);

    void TestReagentListReadSucceed(void);

    void TestReagentListReadFail(void);

    void TestReagentListWriteSucceed(void);

    void TestReagentListWriteFail(void);

    void TestReagentListWriteSucceed2(void);

    void TestReagentListSerialize(void);

    void TestReagentListUpdateReagent(void);

    void TestReagentListAddReagent(void);

    void TestReagentListRemoveReagent(void);

public:
    TestReagentList(void);
};

#endif // TESTREAGENT_H
