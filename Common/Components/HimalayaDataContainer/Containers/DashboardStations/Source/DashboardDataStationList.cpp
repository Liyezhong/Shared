/****************************************************************************/
/*! \file DashboardDataStationList.cpp
 *
 *  \brief DashboardDataStationList class implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16-Jan-2013
 *   $Author:  $ Santhosha Kumar Raju
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2013 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <QDebug>
#include <QFile>
#include <QReadLocker>
#include <QWriteLocker>
#include <QBuffer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QStringList>
#include <QSet>

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Include/DashboardDataStationList.h"
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include "HimalayaDataContainer/Helper/Include/HimalayaDataManagerEventCodes.h"
#include "HimalayaDataContainer/Containers/Reagents/Include/Reagent.h"
#include <QDataStream>

//lint -e429

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Default Constructor
 */
/****************************************************************************/
CDashboardDataStationList::CDashboardDataStationList():
    m_Version(0),
    m_Filename(""),
    m_DataVerificationMode(false),
    mp_ReadWriteLock(NULL)
{
     mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
     Init();
}

CDashboardDataStationList::CDashboardDataStationList(const CDashboardDataStationList& DashboardStationList) :
    CDataContainerBase(),
    m_Version(0),
    m_Filename(""),
    m_DataVerificationMode(false),
    mp_ReadWriteLock(NULL)
{
    try {
        mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
        m_DashboardStationList.clear();
        m_OrderedListOfDashboardStationIDs.clear();
        CopyFromOther(DashboardStationList);
    }
    catch (...){
        //Raise event
    }
}

/****************************************************************************/
/*!
 *  \brief Copy Data from another instance.
 *         This function should be called from CopyConstructor or
 *         Assignment operator only.
.*  \note  Method for internal use only
 *  \param Other
 */
/****************************************************************************/
void CDashboardDataStationList::CopyFromOther(const CDashboardDataStationList &Other)
{
    //QReadWriteLock is not copied. We use the existing lock object
    CDashboardDataStationList &OtherDashboardStationList = const_cast<CDashboardDataStationList &>(Other);
    m_Filename  = OtherDashboardStationList.GetFilename();
    m_Version = OtherDashboardStationList.GetVersion();

    for (qint32 I = 0; I < OtherDashboardStationList.GetNumberOfDashboardStations(); I++)
    {
        (void)AddDashboardStationWithoutVerification(OtherDashboardStationList.GetDashboardStation(I));
    }

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDashboardDataStationList::~CDashboardDataStationList()
{
    try {
        (void)DeleteAllDashboardStations();
    }
    catch(...) {
        //to please PClint
    }
    if (mp_ReadWriteLock != NULL) {
        try {
            delete mp_ReadWriteLock;
        }
        catch(...) {
            //to please PClint
        }
        mp_ReadWriteLock = NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief Start process for CDashboardDataStationList class instance.
 */
/****************************************************************************/
void CDashboardDataStationList::Init()
{
    if(!DeleteAllDashboardStations()) {
        qDebug() << "Error not able to delete dashboard stations";
    }
}

/****************************************************************************/
/*!
 *  \brief Transfer the contents of the StationList class  into a IODevice.
 *
 *  \iparam IODevice = Instance of IODevice Class , buffer or file
 *  \iparam CompleteData = Complete class Data , true or false
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDashboardDataStationList::SerializeContent(QIODevice& IODevice, bool CompleteData)
{
    QString StringValue;
    bool Result = true;
    QXmlStreamWriter XmlStreamWriter;

    XmlStreamWriter.setDevice(&IODevice);
    XmlStreamWriter.setAutoFormatting(true);
    XmlStreamWriter.writeStartDocument();

    XmlStreamWriter.writeDTD("<!DOCTYPE Stations>");
    XmlStreamWriter.writeStartElement("Stations");

    // write attribute version
    (void)StringValue.setNum(GetVersion());
    XmlStreamWriter.writeAttribute("Version", StringValue);

    for(int i=0;i<GetNumberOfDashboardStations();i++)
    {
        CDashboardStation *p_DashboardStation = const_cast<CDashboardStation*> (GetDashboardStation(i));
        if (p_DashboardStation != NULL)
        {
            if (!p_DashboardStation->SerializeContent(XmlStreamWriter, CompleteData))
            {
                qDebug() << "CDashboardDataStationList::SerialiseContent failed";
                return false;
            }
        }
    } // end of for loop
    //======NODE=======Temporary Data Variables=========================
    if ((CompleteData) && (Result)) {
        //Need to be added the temporary members
        XmlStreamWriter.writeStartElement("ClassTemporaryData");

        XmlStreamWriter.writeAttribute("FileName", GetFilename());
        if (GetDataVerificationMode()) {
            XmlStreamWriter.writeAttribute("VerificationMode", "true");
        }
        else {
            XmlStreamWriter.writeAttribute("VerificationMode", "false");
        }
        XmlStreamWriter.writeEndElement(); // for ClassTemporaryData
    }
    //======NODE=END====Temporary Data Variables=========================

    // The above Result has to be true to write the end element
    XmlStreamWriter.writeEndElement(); // for Dashboard Stations
    XmlStreamWriter.writeEndDocument(); // End of Document

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Transfer the contents of the IODevice into the class.
 *
 *  \iparam IODevice = Instance of IODevice Class , buffer or file
 *  \iparam CompleteData = Complete class Data , true or false
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDashboardDataStationList::DeserializeContent(QIODevice& IODevice, bool CompleteData)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(&IODevice);

    // look for node <DashboardStations>
    if (!Helper::ReadNode(XmlStreamReader, "Stations")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: DashboardStations";
        return false;
    }

    // Read attribute Version
    if (!XmlStreamReader.attributes().hasAttribute("Version")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        return false;
    }
    SetVersion(XmlStreamReader.attributes().value("Version").toString().toInt());

    if (!ReadDashboardStations(XmlStreamReader, CompleteData)) {
        qDebug() << "Failure in reading the DashboardStations";
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief This reads the single dashboard station and temporary data
 *
 *  \iparam XmlStreamReader = Instance of the XmlStreamReader
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDashboardDataStationList::ReadDashboardStations(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{
    QStringList NewDashboardStationIDList;
    QSet<QString> OldDashboardStationIDSet =  QSet<QString>::fromList(m_OrderedListOfDashboardStationIDs);

    while (!XmlStreamReader.atEnd()) {
        if (XmlStreamReader.readNext()) {
            if (XmlStreamReader.isStartElement()) {
                if (XmlStreamReader.name() == "Station") {
                    //create a dummy station
                    CDashboardStation Station;

                    if (!Station.DeserializeContent(XmlStreamReader, CompleteData)) {
                        qDebug() << "CDashboardStation::Read failed. ReadStations failed!";
                        return false;
                    }
                    //check if Station is already present
                    CDashboardStation *p_Station = GetDashboardStation(Station.GetDashboardStationID());

                    if (!p_Station) {
                        NewDashboardStationIDList << Station.GetDashboardStationID();
                        // Station not present , now add this Station
                        if (!AddDashboardStation(&Station)) {
                            qDebug() << "CDashboardStation::Add Station failed!";
                            return false;
                        }
                    }
                    else {
                        NewDashboardStationIDList << p_Station->GetDashboardStationID();
                        //Update Station
                        if (!UpdateDashboardStation(&Station)) {
                            qDebug()<<"ReadStations:UpdateStation() Failed";
                            return false;
                        }
                    }
                }
                else if ((XmlStreamReader.name() == "ClassTemporaryData") && (CompleteData)) {
                    // File name
                    if (!XmlStreamReader.attributes().hasAttribute("FileName")) {
                        qDebug() << "### attribute <FileName> is missing => abort reading";
                        return false;
                    }
                    m_Filename = XmlStreamReader.attributes().value("FileName").toString();

                    // VerificationMode
                    if (!XmlStreamReader.attributes().hasAttribute("VerificationMode")) {
                        qDebug() << "### attribute <VerificationMode> is missing => abort reading";
                        return false;
                    } else {
                        bool Value = false;
                        if (XmlStreamReader.attributes().value("VerificationMode").toString().toUpper() == "TRUE") {
                            Value = true;
                        }
                        SetDataVerificationMode(Value);
                    }
                } // end of ClassTemporaryData comparison
                else {
                    qDebug() << "### unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                    return false;
                }
            } // end of start element comparison
            else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "Stations") {
                //qDebug() << "It has reached the end of the particular Node , Stop reading for this Station";
                break; // exit from while loop
            }
        }
    } // end of while loop

    QSet<QString> NewDashboardStationIDSet =  QSet<QString>::fromList(NewDashboardStationIDList);

    //Remove items present in NewStationIDSet from OldStationIDSet
    (void)OldDashboardStationIDSet.subtract(NewDashboardStationIDSet);
    QList<QString> DashboardStationIDToRemove = OldDashboardStationIDSet.toList();

    //Remove station  which is no longer required.
    foreach(QString DashboardStationID, DashboardStationIDToRemove)
    {
        (void)DeleteDashboardStation(DashboardStationID);
    }


    return true;
}

QDataStream& operator <<(QDataStream& OutDataStream, const CDashboardDataStationList& DashboardStationList)
{
    CDashboardDataStationList* p_TempDashboardStationList = const_cast<CDashboardDataStationList*>(&DashboardStationList);

    if (!p_TempDashboardStationList->SerializeContent(*OutDataStream.device(), true)) {
        qDebug() << "CDashboardDataStationList::Operator Streaming (SerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }
    return OutDataStream;
}


QDataStream& operator >>(QDataStream& InDataStream, CDashboardDataStationList& DashboardStationList)
{
    if (!DashboardStationList.DeserializeContent(*InDataStream.device(), true)) {
        qDebug() << "CDashboardDataStationList::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }
    return InDataStream;
}

CDashboardDataStationList& CDashboardDataStationList::operator=(const CDashboardDataStationList &SourceDashboardStationList)
{
    if (this == &SourceDashboardStationList) {
        qDebug() << "CDashboardDataStationList::operator=: SourceDashboardStationList not valid!";
    }

    try {
        (void)DeleteAllDashboardStations();
        (void)CopyFromOther(SourceDashboardStationList);
    }catch (...) {
        (void)DeleteAllDashboardStations();
       //! \todo Inform event Handler
    }

    return (*this);
}

/****************************************************************************/
/*!
 *  \brief Read from the DashboardStation class XML file.
 *
 *  \iparam Filename = XML file name for Station class
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDashboardDataStationList::Read(QString Filename)
{

    if (!QFile::exists(Filename)) {
        return false;
    }

    bool Result = true;

    QWriteLocker locker(mp_ReadWriteLock);

    qDebug() << "CDashboardDataStationList::Read - 1";
    Init();

    // Initialise the m_Filename to a known string "UNDEFINED"
    m_Filename = "UNDEFINED";

    QFile File(Filename);
    if (!File.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "open file failed in Read: " << Filename;
        return false;
    }

    if (!DeserializeContent(File, false)) {
        qDebug() << "### CDashboardStationList::Read failed for file: " << Filename;
        return false;
    }

    //After when the Reading is done an dcopied to Object
    // Copy the filename to the member attribute which can be used while writing
    m_Filename = Filename;

    File.close();

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Gets the dashboard station type based on the station ID
 *
 *  \iparam DashboardStationID = Unique ID of the dashboard station,
 *
 *  \return Dashboard Station type
 */
/****************************************************************************/
DashboardStationType_t CDashboardDataStationList::GetDashboardStationType(const QString DashboardStationID)
{
    CDashboardStation* p_DashboardStation = GetDashboardStation(DashboardStationID);
    if (p_DashboardStation == NULL) {
        return INVALID_STATION;
    }
    return p_DashboardStation->GetDashboardStationType();
}

/****************************************************************************/
/*!
 *  \brief Gets the dashboard station type based on the station index
 *
 *  \iparam Index = Order of Dashboard Station
 *
 *  \return Dashboard Station type
 */
/****************************************************************************/
DashboardStationType_t CDashboardDataStationList::GetDashboardStationType(const unsigned int Index)
{
    CDashboardStation* p_DashboardStation = const_cast<CDashboardStation*> (GetDashboardStation(Index));
    if (p_DashboardStation == NULL) {
        return INVALID_STATION;
    }
    return p_DashboardStation->GetDashboardStationType();
}

/****************************************************************************/
/*!
 *  \brief Gets an instance of the CDashboardStation class.
 *
 *  \iparam Index = Order of Dashboard Station
 *
 *  \return CDashboardStation instance
 */
/****************************************************************************/
CDashboardStation* CDashboardDataStationList::GetDashboardStation(const unsigned int Index)
{
    QReadLocker locker(mp_ReadWriteLock);

    if (Index < (unsigned int)m_OrderedListOfDashboardStationIDs.count()) {
        QString Key = m_OrderedListOfDashboardStationIDs.value(Index);
        CDashboardStation* p_DashboardStation = m_DashboardStationList.value(Key, NULL);
        return p_DashboardStation;
    }

    return NULL;
}

//deep copy
/****************************************************************************/
/*!
 *  \brief Gets the dashboard station based on the station index
 *
 *  \iparam Index = Dashboard Station Index
 *  \iparam DashboardStation = Instance of Station Class
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDashboardDataStationList::GetDashboardStation(const unsigned int Index, CDashboardStation& DashboardStation)
{
    QReadLocker locker(mp_ReadWriteLock);

    bool Result = false;
    if (Index < (unsigned int)m_OrderedListOfDashboardStationIDs.count())
    {
        QString DashboardStationID = m_OrderedListOfDashboardStationIDs.value(Index);
        if (m_DashboardStationList.contains(DashboardStationID))
        {
            DashboardStation = *m_DashboardStationList.value(DashboardStationID);
            Result = true;
        }
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Gets the station based on the station ID
 *
 *  \iparam DashboardStationID = Unique dashboard station ID
 *  \iparam DashboardStation = Instance of Dashboard Station Class
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDashboardDataStationList::GetDashboardStation(const QString DashboardStationID, CDashboardStation& DashboardStation)
{
    QReadLocker locker(mp_ReadWriteLock);
    if (m_DashboardStationList.contains(DashboardStationID))
    {
         DashboardStation = *m_DashboardStationList.value(DashboardStationID);
         return true;
    }
    else
    {
         return false;
    }
}

/****************************************************************************/
/*!
 *  \brief Verifies if the dashboard station exists in the internal list.
 *
 *  \iparam DashboardStationID = Unique station ID
 *
 *  \return TRUE or FALSE
 */
/****************************************************************************/
bool CDashboardDataStationList::IsDashboardStationExists(const QString DashboardStationID)
{
    QReadLocker locker(mp_ReadWriteLock);
    return m_DashboardStationList.contains(DashboardStationID);
}

/****************************************************************************/
/*!
 *  \brief Adds a dashboard station to the internal dashboard station list.
 *
 *  \iparam p_DashboardStation = Instance of Dashboard Station class
 *
 *  \return TRUE or FALSE
 *  \warning if p_DashboardStation is pointing to data in heap, then client is responsible
 *           for memory management. Add Dashboard Station makes a deepcopy of p_DashboardStation
 *           internally
 */
/****************************************************************************/
bool CDashboardDataStationList::AddDashboardStation(const CDashboardStation* p_DashboardStation)
{
    if (p_DashboardStation == NULL) {
        return false;
    }

    QString ID = const_cast<CDashboardStation*>(p_DashboardStation)->GetDashboardStationID();
    if (m_DashboardStationList.contains(ID)) {
        // there is already a dashboard station with the given DashboardStationID
        // => use UpdateDashboardStation instead
        //    or call DeleteDashboardStation before AddDashboardStation
        return false;
    }

    // Here Copy constructor cannot be used ,
    CDashboardStation *p_TempDashboardStation = new CDashboardStation();
    *p_TempDashboardStation = *p_DashboardStation;

    QWriteLocker locker(mp_ReadWriteLock);
    m_DashboardStationList.insert(ID, p_TempDashboardStation);
    m_OrderedListOfDashboardStationIDs.append(ID);

    return true;
}

/****************************************************************************/
/*!
 *  \brief Add Dashboard Station to Dashboard Station List. Doesnt do any verification.
 *
 *  \iparam p_DashboardStation = Instance of Dashboard Station class
 *
 *  \warning  To be used from Assignment operator only
 */
/****************************************************************************/
void CDashboardDataStationList::AddDashboardStationWithoutVerification(const CDashboardStation *p_DashboardStation)
{
    if (p_DashboardStation) {
        QString ID = const_cast<CDashboardStation*>(p_DashboardStation)->GetDashboardStationID();
        // Here Copy constructor cannot be used ,
        CDashboardStation *p_TempDashboardStation = new CDashboardStation();
        // Make a deep copy
        *p_TempDashboardStation = *p_DashboardStation;
        QWriteLocker locker(mp_ReadWriteLock);
        m_DashboardStationList.insert(ID, p_TempDashboardStation);
        m_OrderedListOfDashboardStationIDs.append(ID);
    }
}

/****************************************************************************/
/*!
 *  \brief Updates a exisiting dashboard station in the internal dashboard station list (from GUI).
 *
 *  \iparam p_DashboardStation = Instance of Dashboard Station class
 *
 *  \return TRUE or FALSE
 */
/****************************************************************************/
bool CDashboardDataStationList::UpdateDashboardStation(const CDashboardStation* p_DashboardStation)
{
    if (p_DashboardStation == NULL) {
        return false;
    }

    QString ID = const_cast<CDashboardStation*>(p_DashboardStation)->GetDashboardStationID();
    if (!m_DashboardStationList.contains(ID)) {
        qDebug() << "the station id doesn't exists" << ID;
        return false;
    }

    QWriteLocker locker(mp_ReadWriteLock);
    // do a deep copy
    *m_DashboardStationList[ID] = *p_DashboardStation;
    return true;
}

bool CDashboardDataStationList::UpdateStationsByReagentDelete(const QString& ReagentID)
{
    ListOfDashboardStation_t::const_iterator i = m_DashboardStationList.constBegin();
    while (i != m_DashboardStationList.constEnd()) {
        CDashboardStation*  pDashboardStation = i.value();
        if (pDashboardStation->GetDashboardReagentID() == ReagentID)
        {
            pDashboardStation->SetDashboardReagentID("");
            pDashboardStation->SetDashboardReagentActualCassettes(0);
            pDashboardStation->SetDashboardReagentActualCycles(0);
            pDashboardStation->SetDashboardReagentExchangeDate(QDateTime::fromString("1986-01-01 00:00:00", "yyyy-MM-dd hh:mm:ss"));
            pDashboardStation->SetDashboardReagentStatus("Empty");
        }
        ++i;
     }
    return true;
}



/****************************************************************************/
/*!
 *  \brief Deletes a dashboard station from the internal dashboard station list based on unique ID.
 *
 *  \iparam DashboardStationID = Unique ID of the dashboard station
 *
 *  \return TRUE or FALSE
 */
/****************************************************************************/
bool CDashboardDataStationList::DeleteDashboardStation(const QString DashboardStationID)
{
    try {
        if (m_DashboardStationList.contains(DashboardStationID)) {
            delete m_DashboardStationList.value(DashboardStationID);

            m_DashboardStationList.remove(DashboardStationID);

            int MatchIndex = -1;
            for (int i=0; i<m_OrderedListOfDashboardStationIDs.count(); i++) {
                if (m_OrderedListOfDashboardStationIDs[i] == DashboardStationID) {
                    MatchIndex = i;
                    break;
                }
            }
            // MatchIndex MUST never be -1 !!!
            Q_ASSERT(MatchIndex != -1);
            m_OrderedListOfDashboardStationIDs.removeAt(MatchIndex);

            return true;
        } else {
            // there is no station with the given StationID
            return false;
        }
    }
    catch (...) {
        qDebug() << "### exception in CDataReagentList::DeleteReagent(const QString ReagentID)";
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief Deletes a dashboard station from the internal dashboard station list based on index.
 *
 *  \iparam Index = Unique ID of the dashboard station
 *
 *  \return TRUE or FALSE
 */
/****************************************************************************/
bool CDashboardDataStationList::DeleteDashboardStation(const unsigned int Index)
{
    if (Index < (unsigned int)m_OrderedListOfDashboardStationIDs.count()) {
        QString DashboardStationID = m_OrderedListOfDashboardStationIDs.value(Index);
        return DeleteDashboardStation(DashboardStationID);
    } else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief Deletes all dashboard stations from internal dashboard station list.
 *
 *  \return TRUE or FALSE
 */
/****************************************************************************/
bool CDashboardDataStationList::DeleteAllDashboardStations()
{
    bool Result = true;

    while (m_DashboardStationList.size() > 0) {
        if (DeleteDashboardStation(0) == false) {
            Result = false;
        }
    }

    m_DashboardStationList.clear();
    m_OrderedListOfDashboardStationIDs.clear();

    return Result;
}

} // namespace DataManager
