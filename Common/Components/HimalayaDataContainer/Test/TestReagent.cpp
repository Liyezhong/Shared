/****************************************************************************/
/*! \file TestReagent.cpp
 *
 *  \brief implementation of reagent test.
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

#include "TestReagent.h"

#include <QTest>
#include <QFile>

#include "HimalayaDataManager/Templates/Include/Global.h"
#include "HimalayaDataManager/Containers/Reagent/Include/Reagent.h"
#include "HimalayaDataManager/Containers/Reagent/Include/ReagentList.h"


TestReagentList::TestReagentList(void)
    : QObject()
{
}

void TestReagentList::TestReagentListConstructor(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    // read correct xml file ------------------------------
    const QString validXml("../../Config/ReagentGroups.xml");

    QVERIFY(p_RGL->Read(validXml));

    // create reagent list
    DataManager::ReagentListPtr_t p_ReagentList
            (new DataManager::ReagentList(*p_RGL));

    QVERIFY(p_ReagentList);

    QVERIFY(REAGENTS == p_ReagentList->GetDataContainerType());
}

void TestReagentList::TestReagentListFileVersion(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    // read correct xml file ------------------------------
    const QString validRGXml("../../Config/ReagentGroups.xml");

    QVERIFY(p_RGL->Read(validRGXml));

    // create reagent list
    DataManager::ReagentListPtr_t p_ReagentList
            (new DataManager::ReagentList(*p_RGL));

    QVERIFY(p_ReagentList);

    QVERIFY(p_ReagentList->GetFileVersion().isEmpty());

    const QString version("2");
    p_ReagentList->SetFileVersion(version);
    QVERIFY(version == p_ReagentList->GetFileVersion());
}

void TestReagentList::TestReagentListReadSucceed(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    // read correct xml file ------------------------------
    const QString validRGXml("../../Config/ReagentGroups.xml");

    QVERIFY(p_RGL->Read(validRGXml));

    // create reagent list
    DataManager::ReagentListPtr_t p_ReagentList
            (new DataManager::ReagentList(*p_RGL));

    QVERIFY(p_ReagentList);

    // check empty -----------------------------------
    DataManager::ReagentPtr_t p_Reagent0 = p_ReagentList->GetFirst();

    QVERIFY(!p_Reagent0);

    // read correct xml file ------------------------------
    const QString validXml("../../Config/Reagents.xml");

    QVERIFY(p_ReagentList->Read(validXml));

    // check file name ------------------------------------
    QVERIFY(validXml == p_ReagentList->GetFilename());

    // check next user reagent ID -------------------------
    QVERIFY(110 == p_ReagentList->GetNextUserReagentID());

    // check count ----------------------------------------
    QVERIFY(0 < p_ReagentList->GetCount());

    // check create reagent -------------------------------
    DataManager::ReagentPtr_t p_ReagentNew = p_ReagentList->CreateReagent();

    // check add reagent ----------------------------------
    QVERIFY(!p_ReagentList->AddReagent(DataManager::ReagentPtr_t()));
    QVERIFY(!p_ReagentList->AddReagent(p_ReagentNew));

    // test reagent methods -------------------------------
    QVERIFY(QString("U110") == p_ReagentNew->GetID());
    QVERIFY(!p_ReagentNew->IsReferred());
    QVERIFY(!p_ReagentNew->IsValid());

    // delete
    DataManager::Global::SetSystemIdle(false);
    QVERIFY(!p_ReagentNew->CanDelete());
    DataManager::Global::SetSystemIdle(true);
    p_ReagentNew->AddRef();
    QVERIFY(p_ReagentNew->IsReferred());
    QVERIFY(!p_ReagentNew->CanDelete());
    p_ReagentNew->Release();
    QVERIFY(p_ReagentNew->CanDelete());

    // edit name
    const QString newName("new reagent name");
    DataManager::Global::SetSystemIdle(false);
    QVERIFY(!p_ReagentNew->SetName(newName));
    DataManager::Global::SetSystemIdle(true);
    QVERIFY(!p_ReagentNew->SetName(QString("")));
    QVERIFY(p_ReagentNew->SetName(newName));
    QVERIFY(newName == p_ReagentNew->GetName());
    QVERIFY(!p_ReagentNew->IsValid());

    // edit cassettes until change
    int cassettes = 100;
    DataManager::Global::SetSystemIdle(false);
    QVERIFY(!p_ReagentNew->SetCassettesUntilChange(cassettes));
    DataManager::Global::SetSystemIdle(true);
    QVERIFY(p_ReagentNew->SetCassettesUntilChange(cassettes));
    QVERIFY(cassettes == p_ReagentNew->GetCassettesUntilChange());
    QVERIFY(p_ReagentNew->SetCassettesUntilChange(0));
    QVERIFY(-1 == p_ReagentNew->GetCassettesUntilChange());
    QVERIFY(p_ReagentNew->SetCassettesUntilChange(-10000));
    QVERIFY(-1 == p_ReagentNew->GetCassettesUntilChange());

    // edit cycles until change
    int cycles = 100;
    DataManager::Global::SetSystemIdle(false);
    QVERIFY(!p_ReagentNew->SetCyclesUntilChange(cycles));
    DataManager::Global::SetSystemIdle(true);
    QVERIFY(p_ReagentNew->SetCyclesUntilChange(cycles));
    QVERIFY(cycles == p_ReagentNew->GetCyclesUntilChange());
    QVERIFY(p_ReagentNew->SetCyclesUntilChange(0));
    QVERIFY(-1 == p_ReagentNew->GetCyclesUntilChange());
    QVERIFY(p_ReagentNew->SetCyclesUntilChange(-10000));
    QVERIFY(-1 == p_ReagentNew->GetCyclesUntilChange());

    // edit days until change
    int days = 10;
    DataManager::Global::SetSystemIdle(false);
    QVERIFY(!p_ReagentNew->SetDaysUntilChange(days));
    DataManager::Global::SetSystemIdle(true);
    QVERIFY(p_ReagentNew->SetDaysUntilChange(days));
    QVERIFY(days == p_ReagentNew->GetDaysUntilChange());
    QVERIFY(p_ReagentNew->SetDaysUntilChange(0));
    QVERIFY(-1 == p_ReagentNew->GetDaysUntilChange());
    QVERIFY(p_ReagentNew->SetDaysUntilChange(-10000));
    QVERIFY(-1 == p_ReagentNew->GetDaysUntilChange());

    //edit group
    const QString idRG1("RG1");
    DataManager::ReagentGroupPtr_t p_RG = p_RGL->Find(idRG1);
    QVERIFY(p_RG);
    DataManager::Global::SetSystemIdle(false);
    QVERIFY(!p_ReagentNew->SetGroup(p_RG));
    DataManager::Global::SetSystemIdle(true);
    p_ReagentNew->AddRef();
    QVERIFY(!p_ReagentNew->SetGroup(p_RG));
    p_ReagentNew->Release();
    QVERIFY(p_ReagentNew->SetGroup(p_RG));
    QVERIFY(p_ReagentNew->IsValid());
    QVERIFY(idRG1 == p_ReagentNew->GetGroupID());
    QVERIFY(p_ReagentNew->GetGroup());

    // check add reagent ----------------------------------
    QVERIFY(p_ReagentList->AddReagent(p_ReagentNew));
    QVERIFY(!p_ReagentList->AddReagent(p_ReagentNew));

    // check remove reagent -------------------------------
    QVERIFY(!p_ReagentList->RemoveReagent(QString("NOEXISTID")));
    p_ReagentNew->AddRef();
    QVERIFY(!p_ReagentList->RemoveReagent(p_ReagentNew->GetID()));
    p_ReagentNew->Release();
    QVERIFY(p_ReagentList->RemoveReagent(p_ReagentNew->GetID()));
    QVERIFY(!p_ReagentList->RemoveReagent(p_ReagentNew->GetID()));

    // check interation -----------------------------------
    DataManager::ReagentPtr_t p_Reagent1 = p_ReagentList->GetFirst();
    while (p_Reagent1)
    {
        QVERIFY(p_Reagent1);
        p_Reagent1 = p_ReagentList->GetNext();
    }

    QVERIFY(!p_Reagent1);

    // get at
    for (int i = 0; i < p_ReagentList->GetCount(); ++i)
    {
        QVERIFY(p_ReagentList->GetAt(i));
    }

    QVERIFY(!p_ReagentList->GetAt(p_ReagentList->GetCount()));
    QVERIFY(!p_ReagentList->GetAt(p_ReagentList->GetCount() + 2));
    QVERIFY(!p_ReagentList->GetAt(-1));

    // check find ----------------------------------------
    const QString id1("L1");
    QVERIFY(p_ReagentList->Find(id1));

    const QString id4("U104");
    QVERIFY(p_ReagentList->Find(id4));

    const QString id1000("U1000");
    QVERIFY(!p_ReagentList->Find(id1000));
}

void TestReagentList::TestReagentListReadFail(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    // read correct xml file ------------------------------
    const QString validRGXml("../../Config/ReagentGroups.xml");

    QVERIFY(p_RGL->Read(validRGXml));

    // create reagent list
    DataManager::ReagentListPtr_t p_ReagentList
            (new DataManager::ReagentList(*p_RGL));

    QVERIFY(p_ReagentList);

    // check empty -----------------------------------
    DataManager::ReagentPtr_t p_Reagent0 = p_ReagentList->GetFirst();

    QVERIFY(!p_Reagent0);

    // read correct xml file ------------------------------
    const QString validXml("../../Config/Reagents.xml");

    QVERIFY(p_ReagentList->Read(validXml));

    // test to read non-existing file -----------------------
    const QString nonExistXml("../../Config/NonExistXml.xml");
    if (QFile::exists(nonExistXml))
    {
        QFile::remove(nonExistXml);
    }

    QVERIFY(!p_ReagentList->Read(nonExistXml));

    // check file name
    QVERIFY(validXml == p_ReagentList->GetFilename());

    // test to read non-readable file -----------------------
    const QString nonReadableXml("../../Config/NonReadableXml.xml");

    QFile file(nonReadableXml);
    file.open(QFile::WriteOnly | QFile::Text);
    QFile::Permissions oldPermissions = file.permissions();
    // set the file cannot be read
    file.setPermissions(0);

    QVERIFY(!p_ReagentList->Read(nonReadableXml));

    file.setPermissions(oldPermissions);
    file.close();

    // test to read file without root element ---------------
    const QString noRootXml("../../Config/01ReagentsWithoutRoot.xml");

    QVERIFY(!p_ReagentList->Read(noRootXml));

    // check file name
    QVERIFY(validXml == p_ReagentList->GetFilename());

    // test to read file with wrong root element ------------
    const QString wrongRootXml("../../Config/02ReagentsWrongRoot.xml");

    QVERIFY(!p_ReagentList->Read(wrongRootXml));

    // test to read file without version attribute ----------
    const QString noVersionXml("../../Config/03ReagentsNoVersion.xml");

    QVERIFY(!p_ReagentList->Read(noVersionXml));

    // test to read file with empty version attribute -------
    const QString emptyVersionXml("../../Config/04ReagensEmptyVersion.xml");

    QVERIFY(!p_ReagentList->Read(emptyVersionXml));

    // test to read file without next user reagnent ID attribute
    const QString noNextIDXml("../../Config/05ReagentsNoNextUserReagentID.xml");

    QVERIFY(!p_ReagentList->Read(noNextIDXml));

    // test to read file with wrong next user reagent ID attribute
    const QString wrongNextIDXml
            ("../../Config/06ReagentsWrongNextUserReagentID.xml");

    QVERIFY(!p_ReagentList->Read(wrongNextIDXml));

    // test to read file with wrong elements ----------------
    const QString wrongElementsXml
            ("../../Config/07ReagentsWrongElement.xml");

    QVERIFY(!p_ReagentList->Read(wrongElementsXml));

    // test to read file without ID, name, group ------------
    // cassettes, cycles or days ----------------------------
    const QString noIdXml
            ("../../Config/08ReagentsNoID.xml");

    QVERIFY(!p_ReagentList->Read(noIdXml));

    const QString noNameXml
            ("../../Config/09ReagentsNoName.xml");

    QVERIFY(!p_ReagentList->Read(noNameXml));

    const QString noGroupXml
            ("../../Config/10ReagentsNoGroup.xml");

    QVERIFY(!p_ReagentList->Read(noGroupXml));

    const QString noCassetesXml
            ("../../Config/11ReagentsNoCassettes.xml");

    QVERIFY(!p_ReagentList->Read(noCassetesXml));

    const QString noCyclesXml
            ("../../Config/12ReagentsNoCycles.xml");

    QVERIFY(!p_ReagentList->Read(noCyclesXml));

    const QString noDaysXml
            ("../../Config/13ReagentsNoDays.xml");

    QVERIFY(!p_ReagentList->Read(noDaysXml));

    // test to read file with empty ID, name or color --------
    const QString emptyAttributesXml
            ("../../Config/14ReagentsEmptyIDNameGroup.xml");

    QVERIFY(p_ReagentList->Read(emptyAttributesXml));

    // get file name
    QVERIFY(emptyAttributesXml == p_ReagentList->GetFilename());

    // test to read file with empty cassettes, cycles or days -
    const QString emptyAttributesXml2
            ("../../Config/15ReagentsEmptyRMS.xml");

    QVERIFY(p_ReagentList->Read(emptyAttributesXml2));

    // get file name
    QVERIFY(emptyAttributesXml2 == p_ReagentList->GetFilename());

    // test to read file without elements -------------------
    const QString noElementsXml("../../Config/16ReagentsNoElements.xml");

    QVERIFY(p_ReagentList->Read(noElementsXml));

    // get file name
    QVERIFY(noElementsXml == p_ReagentList->GetFilename());
}

void TestReagentList::TestReagentListWriteSucceed(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    // read correct reagent group xml file ----------------
    const QString validRGXml("../../Config/ReagentGroups.xml");

    QVERIFY(p_RGL->Read(validRGXml));

    // create reagent list
    DataManager::ReagentListPtr_t p_ReagentList
            (new DataManager::ReagentList(*p_RGL));

    QVERIFY(p_ReagentList);

    // read first ------------------------------------------
    const QString validXml("../../Config/ReagentsWrite.xml");

    QVERIFY(p_ReagentList->Read(validXml));

    // get file name
    QVERIFY(validXml == p_ReagentList->GetFilename());

    QVERIFY(p_ReagentList->Write());

    // read back ------------------------------------------
    DataManager::ReagentListPtr_t p_ReagentListCmp
            (new DataManager::ReagentList(*p_RGL));

    QVERIFY(p_ReagentListCmp);

    QVERIFY(p_ReagentListCmp->Read(validXml));

    // get file name
    QVERIFY(validXml == p_ReagentListCmp->GetFilename());

    // verify the result
    QVERIFY(p_ReagentList->GetCount() == p_ReagentListCmp->GetCount());

    DataManager::ReagentPtr_t p_Reagent    = p_ReagentList->GetFirst();
    DataManager::ReagentPtr_t p_ReagentCmp = p_ReagentListCmp->GetFirst();

    while (p_Reagent)
    {
        QVERIFY(p_Reagent->GetID()      == p_ReagentCmp->GetID());
        QVERIFY(p_Reagent->GetName()    == p_ReagentCmp->GetName());
        QVERIFY(p_Reagent->GetGroup()   == p_ReagentCmp->GetGroup());
        QVERIFY(p_Reagent->GetGroupID() == p_ReagentCmp->GetGroupID());
        QVERIFY(p_Reagent->GetCassettesUntilChange() ==
                p_ReagentCmp->GetCassettesUntilChange());
        QVERIFY(p_Reagent->GetCyclesUntilChange()    ==
                p_ReagentCmp->GetCyclesUntilChange());
        QVERIFY(p_Reagent->GetDaysUntilChange()      ==
                p_ReagentCmp->GetDaysUntilChange());

        p_Reagent    = p_ReagentList->GetNext();
        p_ReagentCmp = p_ReagentListCmp->GetNext();
    }
}

void TestReagentList::TestReagentListWriteFail(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    // read correct reagent group xml file ----------------
    const QString validRGXml("../../Config/ReagentGroups.xml");

    QVERIFY(p_RGL->Read(validRGXml));

    // create reagent list
    DataManager::ReagentListPtr_t p_ReagentList
            (new DataManager::ReagentList(*p_RGL));

    QVERIFY(p_ReagentList);

    QVERIFY(!p_ReagentList->Write());
}

void TestReagentList::TestReagentListWriteSucceed2(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    // read correct reagent group xml file ----------------
    const QString validRGXml("../../Config/ReagentGroups.xml");

    QVERIFY(p_RGL->Read(validRGXml));

    // create reagent list
    DataManager::ReagentListPtr_t p_ReagentList
            (new DataManager::ReagentList(*p_RGL));

    QVERIFY(p_ReagentList);

    // read first ------------------------------------------
    const QString validXml("../../Config/ReagentsWrite.xml");

    QVERIFY(p_ReagentList->Read(validXml));

    // get file name
    QVERIFY(validXml == p_ReagentList->GetFilename());
    // check create reagent -------------------------------
    DataManager::ReagentPtr_t p_ReagentNew = p_ReagentList->CreateReagent();

    // edit name
    const QString newName("new reagent name");
    QVERIFY(p_ReagentNew->SetName(newName));
    // edit cassettes until change
    QVERIFY(p_ReagentNew->SetCassettesUntilChange(0));
    // edit cycles until change
    QVERIFY(p_ReagentNew->SetCyclesUntilChange(123));
    // edit days until change
    QVERIFY(p_ReagentNew->SetDaysUntilChange(-10000));
    //edit group
    const QString idRG1("RG1");
    DataManager::ReagentGroupPtr_t p_RG = p_RGL->Find(idRG1);
    QVERIFY(p_ReagentNew->SetGroup(p_RG));

    // check add reagent ----------------------------------
    QVERIFY(p_ReagentList->AddReagent(p_ReagentNew));

    // write to exist xml
    const QString existXml("../../Config/ReagentsWriteToExist.xml");

    QVERIFY(p_ReagentList->Write(existXml));

    // read back ------------------------------------------
    DataManager::ReagentListPtr_t p_ReagentListCmp
            (new DataManager::ReagentList(*p_RGL));

    QVERIFY(p_ReagentListCmp);

    QVERIFY(p_ReagentListCmp->Read(existXml));

    // verify the result
    QVERIFY(p_ReagentList->GetCount() == p_ReagentListCmp->GetCount());

    DataManager::ReagentPtr_t p_Reagent    = p_ReagentList->GetFirst();
    DataManager::ReagentPtr_t p_ReagentCmp = p_ReagentListCmp->GetFirst();

    while (p_Reagent)
    {
        QVERIFY(p_Reagent->GetID()      == p_ReagentCmp->GetID());
        QVERIFY(p_Reagent->GetName()    == p_ReagentCmp->GetName());
        QVERIFY(p_Reagent->GetGroup()   == p_ReagentCmp->GetGroup());
        QVERIFY(p_Reagent->GetGroupID() == p_ReagentCmp->GetGroupID());
        QVERIFY(p_Reagent->GetCassettesUntilChange() ==
                p_ReagentCmp->GetCassettesUntilChange());
        QVERIFY(p_Reagent->GetCyclesUntilChange()    ==
                p_ReagentCmp->GetCyclesUntilChange());
        QVERIFY(p_Reagent->GetDaysUntilChange()      ==
                p_ReagentCmp->GetDaysUntilChange());

        p_Reagent    = p_ReagentList->GetNext();
        p_ReagentCmp = p_ReagentListCmp->GetNext();
    }

    // write to non-exist xml
    const QString nonExistXml("../../Config/ReagentsWriteToNonExist.xml");
    (void)QFile::remove(nonExistXml);

    QVERIFY(p_ReagentList->Write(nonExistXml));

    // read back ------------------------------------------
    DataManager::ReagentListPtr_t p_ReagentListCmp2
            (new DataManager::ReagentList(*p_RGL));

    QVERIFY(p_ReagentListCmp2);

    QVERIFY(p_ReagentListCmp2->Read(existXml));

    // verify the result
    QVERIFY(p_ReagentList->GetCount() == p_ReagentListCmp2->GetCount());

    DataManager::ReagentPtr_t p_Reagent2    = p_ReagentList->GetFirst();
    DataManager::ReagentPtr_t p_ReagentCmp2 = p_ReagentListCmp2->GetFirst();

    while (p_Reagent2)
    {
        QVERIFY(p_Reagent2->GetID()      == p_ReagentCmp2->GetID());
        QVERIFY(p_Reagent2->GetName()    == p_ReagentCmp2->GetName());
        QVERIFY(p_Reagent2->GetGroup()   == p_ReagentCmp2->GetGroup());
        QVERIFY(p_Reagent2->GetGroupID() == p_ReagentCmp2->GetGroupID());
        QVERIFY(p_Reagent2->GetCassettesUntilChange() ==
                p_ReagentCmp2->GetCassettesUntilChange());
        QVERIFY(p_Reagent2->GetCyclesUntilChange()    ==
                p_ReagentCmp2->GetCyclesUntilChange());
        QVERIFY(p_Reagent2->GetDaysUntilChange()      ==
                p_ReagentCmp2->GetDaysUntilChange());

        p_Reagent2    = p_ReagentList->GetNext();
        p_ReagentCmp2 = p_ReagentListCmp2->GetNext();
    }
}

void TestReagentList::TestReagentListSerialize(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    // read correct reagent group xml file ----------------
    const QString validRGXml("../../Config/ReagentGroups.xml");

    QVERIFY(p_RGL->Read(validRGXml));

    // create reagent list
    DataManager::ReagentListPtr_t p_ReagentList
            (new DataManager::ReagentList(*p_RGL));

    QVERIFY(p_ReagentList);

    // read first ------------------------------------------
    const QString validXml("../../Config/ReagentsWrite.xml");

    QVERIFY(p_ReagentList->Read(validXml));

    // serialize to a byte array
    QByteArray byteArray;
    QDataStream out1(&byteArray, QIODevice::ReadWrite);
    out1.setVersion(static_cast<int>(QDataStream::Qt_4_0));

    out1 << (*p_ReagentList);

    // serialize to a file
    const QString serializeXml("../../Config/ReagentsSerialize.xml");
    QFile file(serializeXml);
    file.open(QIODevice::WriteOnly);

    QDataStream out2(&file);
    out2.setVersion(static_cast<int>(QDataStream::Qt_4_0));

    out2 << (*p_ReagentList);

    file.close();

    // read the file back and verify the content ----------
    DataManager::ReagentListPtr_t p_ReagentListCmp
            (new DataManager::ReagentList(*p_RGL));

    QVERIFY(p_ReagentListCmp);

    QVERIFY(p_ReagentListCmp->Read(serializeXml));

    // verify the result
    QVERIFY(p_ReagentList->GetCount() == p_ReagentListCmp->GetCount());

    DataManager::ReagentPtr_t p_Reagent    = p_ReagentList->GetFirst();
    DataManager::ReagentPtr_t p_ReagentCmp = p_ReagentListCmp->GetFirst();

    while (p_Reagent)
    {
        QVERIFY(p_Reagent->GetID()      == p_ReagentCmp->GetID());
        QVERIFY(p_Reagent->GetName()    == p_ReagentCmp->GetName());
        QVERIFY(p_Reagent->GetGroup()   == p_ReagentCmp->GetGroup());
        QVERIFY(p_Reagent->GetGroupID() == p_ReagentCmp->GetGroupID());
        QVERIFY(p_Reagent->GetCassettesUntilChange() ==
                p_ReagentCmp->GetCassettesUntilChange());
        QVERIFY(p_Reagent->GetCyclesUntilChange()    ==
                p_ReagentCmp->GetCyclesUntilChange());
        QVERIFY(p_Reagent->GetDaysUntilChange()      ==
                p_ReagentCmp->GetDaysUntilChange());

        p_Reagent    = p_ReagentList->GetNext();
        p_ReagentCmp = p_ReagentListCmp->GetNext();
    }
}

void TestReagentList::TestReagentListUpdateReagent(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    // read correct xml file ------------------------------
    const QString validRGXml("../../Config/ReagentGroups.xml");

    QVERIFY(p_RGL->Read(validRGXml));

    // create reagent list
    DataManager::ReagentListPtr_t p_ReagentList
            (new DataManager::ReagentList(*p_RGL));

    QVERIFY(p_ReagentList);

    // read correct xml file ------------------------------
    const QString validXml("../../Config/Reagents.xml");

    QVERIFY(p_ReagentList->Read(validXml));

    // update no reagent
    QByteArray baEmpty;
    QDataStream streamEmpty(&baEmpty, QIODevice::ReadOnly);

    QString idEmpty = p_ReagentList->UpdateReagent(streamEmpty);
    QVERIFY(idEmpty.isEmpty());

    // update with empty group
    QByteArray baEmptyGroup("<Reagent ID=\"U107\" "
                            "Name=\"Xylene\" "
                            "Group=\"\" "
                            "CassettesUntilChange=\"600\" "
                            "CyclesUntilChange=\"24\" "
                            "DaysUntilChange=\"21\"/>");
    QDataStream streamEmptyGroup(&baEmptyGroup, QIODevice::ReadOnly);

    QString idEmptyGroup = p_ReagentList->UpdateReagent(streamEmptyGroup);
    QVERIFY(idEmptyGroup.isEmpty());

    // update with empty ID
    QByteArray baEmptyID("<Reagent ID=\"UXYZ\" "
                         "Name=\"Xylene\" "
                         "Group=\"RG2\" "
                         "CassettesUntilChange=\"600\" "
                         "CyclesUntilChange=\"24\" "
                         "DaysUntilChange=\"21\"/>");
    QDataStream streamEmptyID(&baEmptyID, QIODevice::ReadOnly);

    QString idEmptyID = p_ReagentList->UpdateReagent(streamEmptyID);
    QVERIFY(idEmptyID.isEmpty());

    // update with valid ID and color ---------------------
    DataManager::ReagentPtr_t p_Reagent0 = p_ReagentList->GetFirst();

    QVERIFY(p_Reagent0);

    // serialize reagent to a byte array -------------------
    QByteArray baValid;
    QDataStream streamOut(&baValid, QIODevice::WriteOnly);
    streamOut.setVersion(static_cast<int>(QDataStream::Qt_4_0));

    streamOut << (*p_Reagent0);

    QDataStream streamIn(&baValid, QIODevice::ReadOnly);

    DataManager::Global::SetSystemIdle(false);

    QString idFail = p_ReagentList->UpdateReagent(streamIn);
    QVERIFY(idFail.isEmpty());

    DataManager::Global::SetSystemIdle(true);

    streamIn.device()->reset();
    QString idSucceed = p_ReagentList->UpdateReagent(streamIn);
    QVERIFY(idSucceed == p_Reagent0->GetID());
}

void TestReagentList::TestReagentListAddReagent(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    // read correct xml file ------------------------------
    const QString validRGXml("../../Config/ReagentGroups.xml");

    QVERIFY(p_RGL->Read(validRGXml));

    // create reagent list
    DataManager::ReagentListPtr_t p_ReagentList
            (new DataManager::ReagentList(*p_RGL));

    QVERIFY(p_ReagentList);

    // read correct xml file ------------------------------
    const QString validXml("../../Config/Reagents.xml");

    QVERIFY(p_ReagentList->Read(validXml));

    // add no reagent
    QByteArray baEmpty;
    QDataStream streamEmpty(&baEmpty, QIODevice::ReadOnly);

    QString idEmpty = p_ReagentList->AddReagent(streamEmpty);
    QVERIFY(idEmpty.isEmpty());

    // add with empty group
    QByteArray baEmptyGroup("<Reagent Name=\"Xylene\" "
                            "Group=\"\" "
                            "CassettesUntilChange=\"600\" "
                            "CyclesUntilChange=\"24\" "
                            "DaysUntilChange=\"21\"/>");
    QDataStream streamEmptyGroup(&baEmptyGroup, QIODevice::ReadOnly);

    QString idEmptyGroup = p_ReagentList->AddReagent(streamEmptyGroup);
    QVERIFY(idEmptyGroup.isEmpty());

    // fail to set group
    QByteArray baValid("<Reagent Name=\"Xylene\" "
                         "Group=\"RG2\" "
                         "CassettesUntilChange=\"600\" "
                         "CyclesUntilChange=\"24\" "
                         "DaysUntilChange=\"21\"/>");
    QDataStream streamValid(&baValid, QIODevice::ReadOnly);

    DataManager::Global::SetSystemIdle(false);

    QString idFail = p_ReagentList->AddReagent(streamValid);
    QVERIFY(idFail.isEmpty());

    DataManager::Global::SetSystemIdle(true);

    streamValid.device()->reset();
    QString idSucceed = p_ReagentList->AddReagent(streamValid);
    QVERIFY(!idSucceed.isEmpty());
}

void TestReagentList::TestReagentListRemoveReagent(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    // read correct xml file ------------------------------
    const QString validRGXml("../../Config/ReagentGroups.xml");

    QVERIFY(p_RGL->Read(validRGXml));

    // create reagent list
    DataManager::ReagentListPtr_t p_ReagentList
            (new DataManager::ReagentList(*p_RGL));

    QVERIFY(p_ReagentList);

    // read correct xml file ------------------------------
    const QString validXml("../../Config/Reagents.xml");

    QVERIFY(p_ReagentList->Read(validXml));

    // remove no reagent
    QByteArray baEmpty;
    QDataStream streamEmpty(&baEmpty, QIODevice::ReadOnly);

    QString idEmpty = p_ReagentList->RemoveReagent(streamEmpty);
    QVERIFY(idEmpty.isEmpty());

    // remove with invalid ID
    QByteArray baEmptyID("<Reagent ID=\"Xylene\" />");
    QDataStream streamEmptyID(&baEmptyID, QIODevice::ReadOnly);

    QString idEmptyID = p_ReagentList->RemoveReagent(streamEmptyID);
    QVERIFY(idEmptyID.isEmpty());

    // fail to set group
    QByteArray baValid("<Reagent ID=\"L1\"/>");
    QDataStream streamValid(&baValid, QIODevice::ReadOnly);

    QString idSucceed = p_ReagentList->RemoveReagent(streamValid);
    QVERIFY(!idSucceed.isEmpty());
}
