/****************************************************************************/
/*! \file TestReagentGroup.cpp
 *
 *  \brief implementation of reagent group test.
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

#include "TestReagentGroup.h"

#include <QTest>
#include <QByteArray>
#include <QFile>
#include <QDataStream>

#include "HimalayaDataManager/Templates/Include/Global.h"
#include "HimalayaDataManager/Containers/ReagentGroup/Include/ReagentGroup.h"
#include "HimalayaDataManager/Containers/ReagentGroup/Include/ReagentGroupList.h"

TestReagentGroupList::TestReagentGroupList(void)
    : QObject()
{
}

void TestReagentGroupList::TestReagentGroupListConstructor(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    QVERIFY(REAGENT_GROUPS == p_RGL->GetDataContainerType());
}

void TestReagentGroupList::TestReagentGroupListFileVersion(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    QVERIFY(p_RGL->GetFileVersion().isEmpty());

    const QString version("2");
    p_RGL->SetFileVersion(version);
    QVERIFY(version == p_RGL->GetFileVersion());
}

void TestReagentGroupList::TestReagentGroupListReadSucceed(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    // check empty ----------------------------------------
    DataManager::ReagentGroupPtr_t p_RG0 = p_RGL->GetFirst();

    QVERIFY(!p_RG0);

    // read correct xml file ------------------------------
    const QString validXml("../../Config/ReagentGroups.xml");

    QVERIFY(p_RGL->Read(validXml));

    // check file name ------------------------------------
    QVERIFY(validXml == p_RGL->GetFilename());

    // check count ----------------------------------------
    QVERIFY(0 < p_RGL->GetCount());

    // check interation -----------------------------------
    DataManager::ReagentGroupPtr_t p_RG1 = p_RGL->GetFirst();
    while (p_RG1)
    {
        QVERIFY(p_RG1);
        p_RG1 = p_RGL->GetNext();
    }

    QVERIFY(!p_RG1);

    // get at
    for (int i = 0; i < p_RGL->GetCount(); ++i)
    {
        QVERIFY(p_RGL->GetAt(i));
    }

    QVERIFY(!p_RGL->GetAt(p_RGL->GetCount()));
    QVERIFY(!p_RGL->GetAt(p_RGL->GetCount() + 2));
    QVERIFY(!p_RGL->GetAt(-1));

    // check find ----------------------------------------
    const QString id1("RG1");
    QVERIFY(p_RGL->Find(id1));

    const QString id4("RG4");
    QVERIFY(p_RGL->Find(id4));

    const QString id1000("RG1000");
    QVERIFY(!p_RGL->Find(id1000));

    // test Reagent group methods ------------------------
    const QString id2("RG2");
    DataManager::ReagentGroupPtr_t p_RG2 = p_RGL->Find(id2);
    QVERIFY(p_RG2);

    // check ID
    QVERIFY(id2 == p_RG2->GetID());

    // check name
    QVERIFY(!p_RG2->GetName().isEmpty());

    // check can edit color
    QVERIFY(p_RG2->CanEditColor());

    DataManager::Global::SetSystemIdle(false);

    QVERIFY(!p_RG2->CanEditColor());

    // check valid
    QVERIFY(p_RG2->IsValid());

    // check set color
    quint32 color = 0XFF0000;
    QVERIFY(!p_RG2->SetColor(color));

    DataManager::Global::SetSystemIdle(true);

    QVERIFY(p_RG2->SetColor(color));
    QVERIFY(color == p_RG2->GetColor());
}

void TestReagentGroupList::TestReagentGroupListReadFail(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    // read correct xml -------------------------------------
    const QString validXml("../../Config/ReagentGroups.xml");

    QVERIFY(p_RGL->Read(validXml));

    // check file name
    QVERIFY(validXml == p_RGL->GetFilename());

    // test to read non-existing file -----------------------
    const QString nonExistXml("../../Config/NonExistXml.xml");
    if (QFile::exists(nonExistXml))
    {
        QFile::remove(nonExistXml);
    }

    QVERIFY(!p_RGL->Read(nonExistXml));

    // check file name
    QVERIFY(validXml == p_RGL->GetFilename());

    // test to read non-readable file -----------------------
    const QString nonReadableXml("../../Config/NonReadableXml.xml");

    QFile file(nonReadableXml);
    file.open(QFile::WriteOnly | QFile::Text);
    QFile::Permissions oldPermissions = file.permissions();
    // set the file cannot be read
    file.setPermissions(0);

    QVERIFY(!p_RGL->Read(nonReadableXml));

    file.setPermissions(oldPermissions);
    file.close();

    // check file name
    QVERIFY(validXml == p_RGL->GetFilename());

    // test to read file without root element ---------------
    const QString noRootXml("../../Config/01ReagentGroupsWithoutRoot.xml");

    QVERIFY(!p_RGL->Read(noRootXml));

    // check file name
    QVERIFY(validXml == p_RGL->GetFilename());

    // test to read file with wrong root element ------------
    const QString wrongRootXml("../../Config/02ReagentGroupsWrongRoot.xml");

    QVERIFY(!p_RGL->Read(wrongRootXml));

    // test to read file without version attribute ----------
    const QString noVersionXml("../../Config/03ReagentGroupsNoVersion.xml");

    QVERIFY(!p_RGL->Read(noVersionXml));

    // test to read file with empty version attribute -------
    const QString emptyVersionXml("../../Config/04ReagentGroupsEmptyVersion.xml");

    QVERIFY(!p_RGL->Read(emptyVersionXml));

    // test to read file without elements -------------------
    const QString noElementsXml("../../Config/05ReagentGroupsNoElements.xml");

    QVERIFY(p_RGL->Read(noElementsXml));

    // get file name
    QVERIFY(noElementsXml == p_RGL->GetFilename());

    // test to read file with wrong elements ----------------
    const QString wrongElementsXml
            ("../../Config/06ReagentGroupsWrongElements.xml");

    QVERIFY(!p_RGL->Read(wrongElementsXml));

    // test to read file without ID, name or color -----------
    const QString noIdXml
            ("../../Config/07ReagentGroupsNoID.xml");

    QVERIFY(!p_RGL->Read(noIdXml));

    const QString noNameXml
            ("../../Config/08ReagentGroupsNoName.xml");

    QVERIFY(!p_RGL->Read(noNameXml));

    const QString noColorXml
            ("../../Config/09ReagentGroupsNoColor.xml");

    QVERIFY(!p_RGL->Read(noColorXml));

    // test to read file with empty ID, name or color --------
    const QString emptyAttributesXml
            ("../../Config/10ReagentGroupsEmptyIdNameNColor.xml");

    QVERIFY(p_RGL->Read(emptyAttributesXml));

    // get file name
    QVERIFY(emptyAttributesXml == p_RGL->GetFilename());
}

void TestReagentGroupList::TestReagentGroupListWriteSucceed(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    const QString validXml("../../Config/ReagentGroupsWrite.xml");

    QVERIFY(p_RGL->Read(validXml));

    // get file name
    QVERIFY(validXml == p_RGL->GetFilename());

    QVERIFY(p_RGL->Write());

    QSharedPointer<DataManager::ReagentGroupList>
            p_RGLComp(new DataManager::ReagentGroupList());

    QVERIFY(p_RGLComp);

    QVERIFY(p_RGLComp->Read(validXml));

    // get file name
    QVERIFY(validXml == p_RGLComp->GetFilename());

    // verify the result
    QVERIFY(p_RGL->GetCount() == p_RGLComp->GetCount());

    DataManager::ReagentGroupPtr_t p_RG     = p_RGL->GetFirst();
    DataManager::ReagentGroupPtr_t p_RGComp = p_RGLComp->GetFirst();

    while (p_RG)
    {
        QVERIFY(p_RG->GetID()    == p_RGComp->GetID());
        QVERIFY(p_RG->GetName()  == p_RGComp->GetName());
        QVERIFY(p_RG->GetColor() == p_RGComp->GetColor());

        p_RG     = p_RGL->GetNext();
        p_RGComp = p_RGLComp->GetNext();
    }
}

void TestReagentGroupList::TestReagentGroupListWriteFail(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    // write to no file
    QVERIFY(!p_RGL->Write());
}

void TestReagentGroupList::TestReagentGroupListWriteSucceed2(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    const QString validXml("../../Config/ReagentGroupsWrite.xml");

    QVERIFY(p_RGL->Read(validXml));

    // get file name
    QVERIFY(validXml == p_RGL->GetFilename());

    // write to existing file
    const QString existXml("../../Config/ReagentGroupsWriteToExist.xml");

    QVERIFY(p_RGL->Write(existXml));

    QSharedPointer<DataManager::ReagentGroupList>
            p_RGLComp(new DataManager::ReagentGroupList());

    QVERIFY(p_RGLComp);

    QVERIFY(p_RGLComp->Read(existXml));

    // get file name
    QVERIFY(existXml == p_RGLComp->GetFilename());

    // verify the result
    QVERIFY(p_RGL->GetCount() == p_RGLComp->GetCount());

    DataManager::ReagentGroupPtr_t p_RG     = p_RGL->GetFirst();
    DataManager::ReagentGroupPtr_t p_RGComp = p_RGLComp->GetFirst();

    while (p_RG)
    {
        QVERIFY(p_RG->GetID()    == p_RGComp->GetID());
        QVERIFY(p_RG->GetName()  == p_RGComp->GetName());
        QVERIFY(p_RG->GetColor() == p_RGComp->GetColor());

        p_RG     = p_RGL->GetNext();
        p_RGComp = p_RGLComp->GetNext();
    }

    // write to non-existing file
    const QString nonExistXml("../../Config/ReagentGroupsWriteToNonExist.xml");
    (void)QFile::remove(nonExistXml);

    QVERIFY(p_RGL->Write(nonExistXml));

    QSharedPointer<DataManager::ReagentGroupList>
            p_RGLComp2(new DataManager::ReagentGroupList());

    QVERIFY(p_RGLComp2);

    QVERIFY(p_RGLComp2->Read(nonExistXml));

    // get file name
    QVERIFY(nonExistXml == p_RGLComp2->GetFilename());

    // verify the result
    QVERIFY(p_RGL->GetCount() == p_RGLComp2->GetCount());

    DataManager::ReagentGroupPtr_t p_RG2     = p_RGL->GetFirst();
    DataManager::ReagentGroupPtr_t p_RGComp2 = p_RGLComp2->GetFirst();

    while (p_RG2)
    {
        QVERIFY(p_RG2->GetID()    == p_RGComp2->GetID());
        QVERIFY(p_RG2->GetName()  == p_RGComp2->GetName());
        QVERIFY(p_RG2->GetColor() == p_RGComp2->GetColor());

        p_RG2     = p_RGL->GetNext();
        p_RGComp2 = p_RGLComp2->GetNext();
    }
}

void TestReagentGroupList::TestReagentGroupListSerialize(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    const QString validXml("../../Config/ReagentGroups.xml");

    QVERIFY(p_RGL->Read(validXml));

    // serialize to a byte array
    QByteArray byteArray;
    QDataStream out1(&byteArray, QIODevice::ReadWrite);
    out1.setVersion(static_cast<int>(QDataStream::Qt_4_0));

    out1 << (*p_RGL);

    // serialize to a file
    const QString serializeXml("../../Config/ReagentGroupsSerialize.xml");
    QFile file(serializeXml);
    file.open(QIODevice::WriteOnly);

    QDataStream out2(&file);
    out2.setVersion(static_cast<int>(QDataStream::Qt_4_0));

    out2 << (*p_RGL);

    file.close();

    // read the file back and verify the content
    QSharedPointer<DataManager::ReagentGroupList>
            p_RGLComp(new DataManager::ReagentGroupList());

    QVERIFY(p_RGLComp);

    QVERIFY(p_RGLComp->Read(serializeXml));

    // verify the result
    QVERIFY(p_RGL->GetCount() == p_RGLComp->GetCount());

    DataManager::ReagentGroupPtr_t p_RG     = p_RGL->GetFirst();
    DataManager::ReagentGroupPtr_t p_RGComp = p_RGLComp->GetFirst();

    while (p_RG)
    {
        QVERIFY(p_RG->GetID()    == p_RGComp->GetID());
        QVERIFY(p_RG->GetName()  == p_RGComp->GetName());
        QVERIFY(p_RG->GetColor() == p_RGComp->GetColor());

        p_RG     = p_RGL->GetNext();
        p_RGComp = p_RGLComp->GetNext();
    }
}

void TestReagentGroupList::TestReagentGroupListUpdateReagentGroup(void)
{
    DataManager::ReagentGroupListPtr_t p_RGL
            (new DataManager::ReagentGroupList());

    QVERIFY(p_RGL);

    // read correct xml file ------------------------------
    const QString validXml("../../Config/ReagentGroups.xml");

    QVERIFY(p_RGL->Read(validXml));

    // update no reagent group
    QByteArray baEmpty;
    QDataStream streamEmpty(&baEmpty, QIODevice::ReadOnly);

    QString idEmpty = p_RGL->UpdateReagentGroup(streamEmpty);
    QVERIFY(idEmpty.isEmpty());

    // update with empty color
    QByteArray baEmptyColor("<ReagentGroup ID=\"RG1\" Color=\"\"/>");
    QDataStream streamEmptyColor(&baEmptyColor, QIODevice::ReadOnly);

    QString idEmptyColor = p_RGL->UpdateReagentGroup(streamEmptyColor);
    QVERIFY(idEmptyColor.isEmpty());

    // update with empty ID
    QByteArray baEmptyID("<ReagentGroup ID=\"\" Color=\"FF5566\"/>");
    QDataStream streamEmptyID(&baEmptyID, QIODevice::ReadOnly);

    QString idEmptyID = p_RGL->UpdateReagentGroup(streamEmptyID);
    QVERIFY(idEmptyID.isEmpty());

    // update with valid ID and color ---------------------
    DataManager::ReagentGroupPtr_t p_RG0 = p_RGL->GetFirst();

    QVERIFY(p_RG0);

    // check color and serialize --------------------------
    quint32 color = p_RG0->GetColor();
    p_RG0->SetColor(~color);

    // serialize to a byte array
    QByteArray baValid;
    QDataStream streamOut(&baValid, QIODevice::WriteOnly);
    streamOut.setVersion(static_cast<int>(QDataStream::Qt_4_0));

    streamOut << (*p_RG0);

    p_RG0->SetColor(color);

    QDataStream streamIn(&baValid, QIODevice::ReadOnly);

    DataManager::Global::SetSystemIdle(false);

    QString idFail = p_RGL->UpdateReagentGroup(streamIn);
    QVERIFY(idFail.isEmpty());

    DataManager::Global::SetSystemIdle(true);

    streamIn.device()->reset();
    QString idSucceed = p_RGL->UpdateReagentGroup(streamIn);
    QVERIFY(idSucceed == p_RG0->GetID());
    QVERIFY(~color == p_RG0->GetColor());
}
