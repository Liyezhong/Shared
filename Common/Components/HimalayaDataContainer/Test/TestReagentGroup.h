/****************************************************************************/
/*! \file TestReagentGroup.h
 *
 *  \brief header of reagent group test.
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

#ifndef TESTREAGENTGROUP_H
#define TESTREAGENTGROUP_H

#include <QObject>


class TestReagentGroupList : public QObject
{
    Q_OBJECT
private slots:
    void TestReagentGroupListConstructor(void);

    void TestReagentGroupListFileVersion(void);

    void TestReagentGroupListReadSucceed(void);

    void TestReagentGroupListReadFail(void);

    void TestReagentGroupListWriteSucceed(void);

    void TestReagentGroupListWriteFail(void);

    void TestReagentGroupListWriteSucceed2(void);

    void TestReagentGroupListSerialize(void);

    void TestReagentGroupListUpdateReagentGroup(void);

public:
    TestReagentGroupList(void);
};

#endif // TESTREAGENTGROUP_H
