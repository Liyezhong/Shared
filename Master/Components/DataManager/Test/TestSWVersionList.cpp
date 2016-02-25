/****************************************************************************/
/*! \file TestSWVersionList.cpp
 *
 *  \brief Implementation file for class TestSWVersionList.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-09-07
 *  $Author:    $ Raju
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

#include <QTest>
#include <QFile>
#include "DataManager/Containers/SWVersions/Include/SWVersionList.h"
#include "DataManager/Containers/SWVersions/Include/SWVersionListVerifier.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"


namespace DataManager {

const QString RESOURCE_FILENAME = ":/Xml/SW_Version.xml"; ///< Resource file path

/****************************************************************************/
/**
 * \brief Test class for Xml SW_version class.
 */
/****************************************************************************/
class TestSWVersionList : public QObject {
    Q_OBJECT
private slots:
    /****************************************************************************/
    /**
     * \brief Called before the first testfunction is executed.
     */
    /****************************************************************************/
    void initTestCase();
    /****************************************************************************/
    /**
     * \brief Called before each testfunction is executed.
     */
    /****************************************************************************/
    void init();
    /****************************************************************************/
    /**
     * \brief Called after each testfunction was executed.
     */
    /****************************************************************************/
    void cleanup();
    /****************************************************
************************/
    /**
     * \brief Called after last testfunction was executed.
     */
    /****************************************************************************/
    void cleanupTestCase();
    /****************************************************************************/
    /**
     * \brief Test the SW details.
     */
    /****************************************************************************/
    void utTestSWDetails();

    /****************************************************************************/
    /**
     * \brief Test write and read SW version list container.
     */
    /****************************************************************************/
    void utTestSWVersionList();

    /****************************************************************************/
    /**
     * \brief Test write and read SW version list verifier.
     */
    /****************************************************************************/
    void utTestSWVersionListVerifier();

}; // end class TestSWVersionList

/****************************************************************************/
void TestSWVersionList::initTestCase() {    
}

/****************************************************************************/
void TestSWVersionList::init() {
}

/****************************************************************************/
void TestSWVersionList::cleanup() {
}

/****************************************************************************/
void TestSWVersionList::cleanupTestCase() {

}

/****************************************************************************/
void TestSWVersionList::utTestSWDetails() {
    CSWDetails SWDetails1;
    // set all the parameters
    SWDetails1.SetSWDate("2012-01-12");
    SWDetails1.SetSWVersion("MAIN_1.123");
    SWDetails1.SetSWName("Colorado");
    SWDetails1.SetSWType(MASTERSOFTWARE);

    // compare the data
    QCOMPARE(SWDetails1.GetSWDate(), QString("2012-01-12"));
    QCOMPARE(SWDetails1.GetSWVersion(), QString("MAIN_1.123"));
    QCOMPARE(SWDetails1.GetSWName(), QString("Colorado"));
    QCOMPARE(SWDetails1.GetSWType(), (SWType_t) MASTERSOFTWARE);

    // check the copy constructor
    CSWDetails SWDetails2 = SWDetails1;

    // compare the data with other class data
    QCOMPARE(SWDetails1.GetSWDate(), SWDetails2.GetSWDate());
    QCOMPARE(SWDetails1.GetSWVersion(), SWDetails2.GetSWVersion());
    QCOMPARE(SWDetails1.GetSWName(), SWDetails2.GetSWName());
    QCOMPARE(SWDetails1.GetSWType(), SWDetails2.GetSWType());

    // change the SWDetails2 class
    SWDetails2.SetSWDate("2012-06-22");
    SWDetails2.SetSWVersion("SABB_1.123");
    SWDetails2.SetSWName("SABb.bin");
    SWDetails2.SetSWType(FIRMWARE);

    // check the assignment operator
    CSWDetails SWDetails3;
    SWDetails3 = SWDetails2;

    // compare the data with other class data
    QCOMPARE(SWDetails3.GetSWDate(), SWDetails2.GetSWDate());
    QCOMPARE(SWDetails3.GetSWVersion(), SWDetails2.GetSWVersion());
    QCOMPARE(SWDetails3.GetSWName(), SWDetails2.GetSWName());
    QCOMPARE(SWDetails3.GetSWType(), SWDetails2.GetSWType());
}

/****************************************************************************/
void TestSWVersionList::utTestSWVersionList() {
    CSWVersionList *VersionList= new CSWVersionList();
    QString FilesPathWrite;

    // store the application path to write the test files
    FilesPathWrite = QCoreApplication::applicationDirPath() + "/";

    // get the container type
    QCOMPARE(VersionList->GetDataContainerType(), SWVERSION);
    // by default no file name will be available
    QCOMPARE(VersionList->GetFilename(), QString(""));

    CSWDetails* SWDetails1 = new CSWDetails();
    // set all the parameters
    SWDetails1->SetSWDate("2012-01-12");
    SWDetails1->SetSWVersion("MAIN_1.123");
    SWDetails1->SetSWName("Colorado");
    SWDetails1->SetSWType(MASTERSOFTWARE);
    // no verification is required
    VersionList->SetDataVerificationMode(false);

    QCOMPARE(VersionList->GetDataVerificationMode(), false);

    // add sw details to the list
    VersionList->AddSWDetails(SWDetails1);
    QCOMPARE(VersionList->GetNumberOfSWDetails(), 1);
    CSWVersionList VersionList4;
    VersionList4 = *VersionList;

    // check the content of the data from the list
    CSWDetails SWDetails;
    QCOMPARE(VersionList->GetSWDetails("Colorado", SWDetails), true);
    // compare the data with other class data
    QCOMPARE(SWDetails.GetSWDate(), SWDetails1->GetSWDate());
    QCOMPARE(SWDetails.GetSWVersion(), SWDetails1->GetSWVersion());
    QCOMPARE(SWDetails.GetSWName(), SWDetails1->GetSWName());
    QCOMPARE(SWDetails.GetSWType(), SWDetails1->GetSWType());



    CSWDetails* SWDetails2 = new CSWDetails();
    // change the SWDetails2 class
    SWDetails2->SetSWDate("2012-06-22");
    SWDetails2->SetSWVersion("SABB_1.123");
    SWDetails2->SetSWName("SABb.bin");
    SWDetails2->SetSWType(FIRMWARE);
    // write the test files
    VersionList->Write(FilesPathWrite + "SW_Version_Test.xml");

    // add to the list
    VersionList->AddSWDetails(SWDetails2);
    QCOMPARE(VersionList->GetNumberOfSWDetails(), 2);
    QCOMPARE(VersionList->GetSWDetails("NEW", SWDetails), false);

    QCOMPARE(VersionList->GetSWDetails("SABb.bin", SWDetails), true);

    // compare the data with other class data
    QCOMPARE(SWDetails.GetSWDate(), SWDetails2->GetSWDate());
    QCOMPARE(SWDetails.GetSWVersion(), SWDetails2->GetSWVersion());
    QCOMPARE(SWDetails.GetSWName(), SWDetails2->GetSWName());
    QCOMPARE(SWDetails.GetSWType(), SWDetails2->GetSWType());

    // update the sw details
    SWDetails2->SetSWVersion("SABB_1.123");
    SWDetails2->SetSWName("SABb.xxx");
    QCOMPARE(VersionList->UpdateSWDetails(SWDetails2), false);
    QCOMPARE(VersionList->GetSWDetails("SABb.bin", SWDetails), true);
    QCOMPARE(SWDetails.GetSWVersion(), SWDetails2->GetSWVersion());

    // remove the sw details
    VersionList->DeleteSWDetails(1);
    QCOMPARE(VersionList->GetNumberOfSWDetails(), 1);

    QCOMPARE(VersionList->GetSWDetails("MAIN_1.123", SWDetails), false);

   // CSWVersionList *VersionList1= new CSWVersionList(*VersionList);
    CSWVersionList *VersionList3 = new CSWVersionList();
    VersionList3 = VersionList;
    // create the datastream and check whether copying data is working or not
    QByteArray* p_TempByteArray = new QByteArray();
    QDataStream DataStream(p_TempByteArray, QIODevice::ReadWrite);
    DataStream.setVersion(QDataStream::Qt_4_0);
    p_TempByteArray->clear();
    DataStream << *VersionList3;
    DataStream.device()->reset();
    DataStream >> *VersionList;
    QCOMPARE(VersionList3->GetNumberOfSWDetails(), VersionList->GetNumberOfSWDetails());


    // delete the pointers
    delete SWDetails1;
    delete SWDetails2;

    // remove the file
    QFile::remove(FilesPathWrite + "SW_Version_Test.xml");
}

/****************************************************************************/
void TestSWVersionList::utTestSWVersionListVerifier() {
    CSWVersionList VersionList;
    IVerifierInterface *p_SWVersionListVerifier;
    p_SWVersionListVerifier = new CSWVersionListVerifier();
    // add verifier to the list
    VersionList.AddVerifier(p_SWVersionListVerifier);

    VersionList.SetDataVerificationMode(false);
    QCOMPARE(VersionList.Read(RESOURCE_FILENAME), false);
    VersionList.SetDataVerificationMode(true);
    // do verification of the container
    QCOMPARE(VersionList.VerifyData(), true);
    // check the content of the data from the list
    CSWDetails SWDetails;
    QCOMPARE(VersionList.GetSWDetails("SABb.bin", SWDetails), true);
    QCOMPARE(VersionList.GetSWDetails("Himalaya", SWDetails), true);
    QCOMPARE(VersionList.GetSWDetails("BLG", SWDetails), true);
    QCOMPARE(VersionList.GetSWDetails("ASB1.bin", SWDetails), true);
    QCOMPARE(VersionList.GetSWDetails("RemoteCare", SWDetails), true);
    QCOMPARE(VersionList.GetSWDetails("ASB12.bin", SWDetails), true);
    QCOMPARE(VersionList.GetSWDetails("ASB15.bin", SWDetails), false);
    QCOMPARE(VersionList.GetSWDetails("Hello", SWDetails), false);
    QCOMPARE(VersionList.GetSWDetails("No SW", SWDetails), false);
}

} // end namespace DataManagement

QTEST_MAIN(DataManager::TestSWVersionList)

#include "TestSWVersionList.moc"
