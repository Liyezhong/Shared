/****************************************************************************/
/*! \file ExportConfiguration.cpp
 *
 *  \brief Implementation file for class CExportConfiguration.
 *         This class reads the "ExportConfiguration.xml" file and
 *         creates the service and user configuration classes
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-25
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

#include <QDebug>
#include <QString>
#include <QBuffer>
#include <QXmlStreamReader>
#include <iostream>

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "TestStubDataManager/Containers/ExportConfiguration/Include/ExportConfiguration.h"


namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Default Constructor
 *
 */
/****************************************************************************/
CExportConfiguration::CExportConfiguration()  : CDataContainerBase(),
    m_SourceDir(""),
    m_TargetDir(""),
    m_TargetFileName(""),
    m_Version(1),
    m_UserConfigurationFlag(false),
    m_ServiceConfigurationFlag(false),
    m_DataVerificationMode(true),
    m_Filename("")
{    
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam ExportConfiguration = Instance of the CExportConfiguration class
 *
 *  \return
 */
/****************************************************************************/
CExportConfiguration::CExportConfiguration(const CExportConfiguration& ExportConfiguration)  :
    CDataContainerBase(),
    m_SourceDir(""),
    m_TargetDir(""),
    m_TargetFileName(""),
    m_Version(1),
    m_UserConfigurationFlag(false),
    m_ServiceConfigurationFlag(false),
    m_DataVerificationMode(true),
    m_Filename("")
{
    CExportConfiguration* p_TempExportConfiguration = const_cast<CExportConfiguration*>(&ExportConfiguration);

    *this = *p_TempExportConfiguration;
}

/****************************************************************************/
/*!
 *  \brief Writes from the CExportConfiguration object to a IODevice.
 *
 *  \iparam IODevice = Instance of the QIODevice
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CExportConfiguration::SerializeContent(QIODevice& IODevice, bool CompleteData)
{
    QXmlStreamWriter XmlStreamWriter; ///< Xml stream writer object to write the Xml contents in a file

    XmlStreamWriter.setDevice(&IODevice);
    XmlStreamWriter.setAutoFormatting(true);
    //XmlStreamWriter.setAutoFormattingIndent(4);
    // write the document
    XmlStreamWriter.writeStartDocument();

    // start the xml element
    XmlStreamWriter.writeStartElement("ExportConfiguration");
    XmlStreamWriter.writeAttribute("Version", QString::number(GetVersion()));
    // write the souce directory path
    XmlStreamWriter.writeStartElement("Source");
    XmlStreamWriter.writeAttribute("Directory", GetSourceDir());
    XmlStreamWriter.writeEndElement(); //Source

    // write the destination directory path
    XmlStreamWriter.writeStartElement("Destination");
    XmlStreamWriter.writeAttribute("Directory", GetTargetDir());
    XmlStreamWriter.writeAttribute("Filename", GetTargetFileName());
    XmlStreamWriter.writeEndElement(); //Destination

    // serialize the service configuration data
    if (!m_ServiceConfiguration.SerializeContent(XmlStreamWriter, CompleteData)) {
        /// \todo need to add error code
        return false;
    }

    // serialize the service configuration data
    if (!m_UserConfiguration.SerializeContent(XmlStreamWriter, CompleteData)) {
        /// \todo need to add error code
        return false;
    }

    //======NODE=======Temporary Data Variables=========================
    if(CompleteData) {
        // need to be added the temporary members
        XmlStreamWriter.writeStartElement("ClassTemporaryData");
        // store user configuration flag
        if (GetUserConfigurationFlag()) {
            XmlStreamWriter.writeAttribute("UserConfigFlag", "true");
        }
        else {
            XmlStreamWriter.writeAttribute("UserConfigFlag", "false");
        }

        // store service configuration flag
        if (GetServiceConfigurationFlag()) {
            XmlStreamWriter.writeAttribute("ServiceConfigFlag", "true");
        }
        else {
            XmlStreamWriter.writeAttribute("ServiceConfigFlag", "false");
        }

        //        // store the verifier count
        //        XmlStreamWriter.writeAttribute("VerifierCount", QString::number(m_VerifierList.count()));
        //        for (qint32 I = 0; I < m_VerifierList.count(); I++) {
        //            QString VerifierPtr;
        //            // make the text stream input as string
        //            QTextStream VerifierPointer(&VerifierPtr);
        //            // get the verifier list pointer address
        //            VerifierPointer << m_VerifierList.at(I);
        //            qDebug() << "\n\n Real Verifier Pointer" << m_VerifierList.at(I);
        //            XmlStreamWriter.writeAttribute(QString("Verifier%1Pointer").arg(I + 1), VerifierPointer.readAll());
        //        }
        // check for the error string
        //        if (m_ErrorList.count() > 0) {
        //            // start the special tag for error list to store
        //            XmlStreamWriter.writeStartElement("ErrorData");
        //            for (int ErrorListCount = 0; ErrorListCount < m_ErrorList.count(); ErrorListCount++) {
        //                // start of the error string
        //                XmlStreamWriter.writeStartElement("Error");
        //                // write the description of the error string
        //                XmlStreamWriter.writeAttribute("Description", m_ErrorList.value(ErrorListCount));
        //                // end of the error string
        //                XmlStreamWriter.writeEndElement();
        //            }
        //            // end element for the error data
        //            XmlStreamWriter.writeEndElement();
        //        }

        XmlStreamWriter.writeEndElement(); // for ClassTemporaryData

    }
    //======NODE=END====Temporary Data Variables=========================

    XmlStreamWriter.writeEndElement(); //ExportConfiguration
    // write enddocument
    XmlStreamWriter.writeEndDocument();

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads from the XmlStreamReader to CExportConfiguration object.
 *
 *  \iparam IODevice = Instance of the QIODevice.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CExportConfiguration::DeserializeContent(QIODevice& IODevice, bool CompleteData)
{
    QXmlStreamReader XmlStreamReader;

    XmlStreamReader.setDevice(&IODevice);
    // read the <ExportConfiguration> tag
    if(!Helper::ReadNode(XmlStreamReader, "ExportConfiguration")) {
        return false;
    }

    // read the version attribute
    if (!XmlStreamReader.attributes().hasAttribute("Version")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        return false;
    }
    SetVersion(XmlStreamReader.attributes().value("Version").toString().toUInt());

    // read the <Source> tag
    if(!Helper::ReadNode(XmlStreamReader, "Source")) {
        return false;
    }
    // read the Directory attribute
    if (!XmlStreamReader.attributes().hasAttribute("Directory")) {
        qDebug() << "### attribute <Directory> is missing => abort reading";
        return false;
    }
    SetSourceDir(XmlStreamReader.attributes().value("Directory").toString());

    // read the <Destination> tag
    if(!Helper::ReadNode(XmlStreamReader, "Destination")) {
        return false;
    }

    // read the Directory attribute
    if (!XmlStreamReader.attributes().hasAttribute("Directory")) {
        qDebug() << "### attribute <Directory> is missing => abort reading";
        return false;
    }
    SetTargetDir(XmlStreamReader.attributes().value("Directory").toString());

    // read the Filename attribute
    if (!XmlStreamReader.attributes().hasAttribute("Filename")) {
        qDebug() << "### attribute <Filename> is missing => abort reading";
        return false;
    }
    SetTargetFileName(XmlStreamReader.attributes().value("Filename").toString());

    // desrialize the content
    if (!m_ServiceConfiguration.DeserializeContent(XmlStreamReader, CompleteData)) {
        /// \todo need to write error code here
        return false;
    }
    // deserialize the content
    if (!m_UserConfiguration.DeserializeContent(XmlStreamReader, CompleteData)) {
        /// \todo need to write error code here
        return false;
    }

    //======NODE=======Temporary Data Variables=========================
    if (CompleteData) {
        // read the <ClassTemporaryData> tag
        if(!Helper::ReadNode(XmlStreamReader, "ClassTemporaryData")) {
            return false;
        }
        // UserConfigFlag
        if (!XmlStreamReader.attributes().hasAttribute("UserConfigFlag")) {
            qDebug() << "CExportConfiguration::### attribute <UserConfigFlag> is missing => abort reading";
            return false;
        }
        else {
            bool Value = false;
            if (XmlStreamReader.attributes().value("UserConfigFlag").toString().toUpper() == "TRUE") {
                Value = true;
            }
            SetUserConfigurationFlag(Value);
        }
        // ServiceConfigFlag
        if (!XmlStreamReader.attributes().hasAttribute("ServiceConfigFlag")) {
            qDebug() << "CExportConfiguration::### attribute <ServiceConfigFlag> is missing => abort reading";
            return false;
        }
        else {
            bool Value = false;
            if (XmlStreamReader.attributes().value("ServiceConfigFlag").toString().toUpper() == "TRUE") {
                Value = true;
            }
            SetServiceConfigurationFlag(Value);
        }

        //                    // get the verifier count
        //                    int VerifierCount = XmlStreamReader.attributes().value("VerifierCount").toString().toInt();
        //                    // clear the list
        //                    m_VerifierList.clear();
        //
        //                    // start adding the verifiers
        //                    for (qint32 I = 0; I < VerifierCount; I++) {
        //                        if (!XmlStreamReader.attributes().hasAttribute(QString("Verifier%1Pointer").arg(I + 1))) {
        //                            qDebug() << "### attribute <VerifierPointer> is missing";
        //                            return false;
        //                        }
        //                        // get the verifier interface address from the XML file
        //                        IVerifierInterface* p_VerifierInterface = reinterpret_cast<IVerifierInterface*>(XmlStreamReader.attributes().
        //                                                                                                   value(QString("Verifier%1Pointer").
        //                                                                                                         arg(I + 1)).toString().toInt(0, 16));
        //                        qDebug() << "\n\n Verifier Pointer" << p_VerifierInterface;
        //                        // append the verifier
        //                        m_VerifierList.append(p_VerifierInterface);
        //                    }

        // this node is not a compulsory node, so we don't require to return
        // unsuccessful(false)
        //                    if (Helper::ReadNode(XmlStreamReader, "ErrorData")) {
        //                        ReadErrorList(XmlStreamReader);
        //                    }
    }
    //======NODE=END====Temporary Data Variables=========================

    return true;
}


/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam ExportConfiguration = CExportConfiguration class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CExportConfiguration& ExportConfiguration)
{
    CExportConfiguration* p_TempExportConfiguration = const_cast<CExportConfiguration*>(&ExportConfiguration);

    if (!p_TempExportConfiguration->SerializeContent(*OutDataStream.device(), true)) {
        qDebug() << "CExportConfiguration::Operator Streaming (SerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }
    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the QDataStream
 *  \iparam ExportConfiguration = CExportConfiguration class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CExportConfiguration& ExportConfiguration)
{
    if (!ExportConfiguration.DeserializeContent(*InDataStream.device(), true)) {
        qDebug() << "CExportConfiguration::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceExportConfiguration = CExportConfiguration class object
 *
 *  \return CExportConfiguration Class Object
 */
/****************************************************************************/
CExportConfiguration& CExportConfiguration::operator=(const CExportConfiguration& SourceExportConfiguration)
{
    // make sure not same object
    if (this != &SourceExportConfiguration)
    {
        QByteArray TempByteArray;
        CExportConfiguration* p_TempExportConfiguration = const_cast<CExportConfiguration*>(&SourceExportConfiguration);
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        (void)DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0)); //to avoid lint-534
        TempByteArray.clear();
        DataStream << *p_TempExportConfiguration;
        (void)DataStream.device()->reset(); //to avoid lint-534
        DataStream >> *this;
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief Reads the data and stores in CExportConfiguration from the file
 *
 *  \iparam Filename = Name of the Input File
 *
 *  \return  Successful (true) or not (false)
 */
/****************************************************************************/
bool CExportConfiguration::Read(QString Filename)
{
    //check if file exists
    if (!QFile::exists(Filename)) {
        qDebug("File doesn't exists");
        return false;
    }
    bool Result = true;

    if (m_DataVerificationMode) {
//        QWriteLocker locker(mp_ReadWriteLock);
        // create instance of CExportConfiguration for verification
        CExportConfiguration* p_EC_Verification = new CExportConfiguration();
        *p_EC_Verification = *this;
        // disable verification in clone
        p_EC_Verification->SetDataVerificationMode(false);

        // execute required action (Read) in clone
        Result = true;
        if (!p_EC_Verification->Read(Filename)) {
            Result = false;
        } else {
            // now check new content => call all active verifiers
            if (DoLocalVerification(p_EC_Verification)) {
                // if content ok, clone backwards
                *this = *p_EC_Verification;
            } else {
                Result = false;
            }
        }
        // delete test clone
        delete p_EC_Verification;

    } else {
//        QWriteLocker locker(mp_ReadWriteLock);

        m_Filename = "UNDEFINED";
        QFile File (Filename);
        if (!File.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "open file failed in Read: " << Filename;
            return false;
        }
        if (!DeserializeContent(File, false)) {
            qDebug() << "### CExportConfiguration::Read failed for file: " << Filename;
            return false;
        }
        File.close();

        //Now initialize the filename member since Read is succcess
        m_Filename = Filename;

        Result = true;
    }
    return Result;
}

}  // namespace DataManager

