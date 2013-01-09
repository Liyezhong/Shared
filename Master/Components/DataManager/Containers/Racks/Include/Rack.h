/****************************************************************************/
/*! \file Rack.h
 *
 *  \brief CRack definition.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 2012-01-02, 2012-04-23
 *   $Author:  $ M.Scherer, Raju
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

#ifndef DATAMANAGER_RACK_H
#define DATAMANAGER_RACK_H

#include <QDateTime>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QDebug>

//lint -sem(DataManager::CRack::SetRFIDUserData,initializer)

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  This class implements a rack data item.
 */
/****************************************************************************/
class CRack
{
    friend class CRackList;

protected:
    quint32 m_RFIDUniqueID;                 ///< Rack RFID unique ID
    quint32 m_RFIDUserData;                 ///< Rack RFID user data
    QString m_ProgramID;                    ///< Program ID
    bool m_Transported;                     ///< Rack mode (transport/station)
    bool m_SepiaStation;                    ///< Is the rack in Sepia?
    bool m_UseSepia;                        ///< Is the rack relevant for Sepia ?
    QDateTime m_StartTime;                  ///< Scheduler claculation time
    QDateTime m_EndTimeColorado;            ///< End time Colorado
    QString m_StationID;                    ///< Station ID
    quint16 m_ProgramStepIndex;             ///< Current program step
    QDateTime m_EndTimeStation;             ///< Station end time
    QString m_Color;                        ///< Color
    QDateTime m_EndTimeSepia;               ///< End time Sepia
    bool m_Orientation;                     ///< Orientation of the rack
    quint32 m_ActualSlides;                 ///< Actual number of slides in the station. Updated by scheduler
    quint32 m_ActualSlideForecastValue;     ///< Actual number of future slides. Updated by scheduler
    quint32 m_RMSForecastValue;     ///< Slide number (forecast value) for this rack
public:

    CRack(const quint32 RFIDUniqueID, const quint32 RFIDUserData);
    CRack(const CRack&);

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CRack& Rack);
    friend QDataStream& operator >>(QDataStream& InDataStream, CRack& Rack);
    CRack& operator=(const CRack&);

    /****************************************************************************/
    /*!
     *  \brief  Get the Rack RFID
     *
     *  \return integer
     */
    /****************************************************************************/
    quint32 GetRFIDUniqueID() const { return m_RFIDUniqueID; }

    /****************************************************************************/
    /*!
     *  \brief  Get the Rack RFID user data
     *
     *  \return integer
     */
    /****************************************************************************/
    quint32 GetRFIDUserData() const { return m_RFIDUserData; }

    /****************************************************************************/
    /*!
     *  \brief  Get the Rack program ID
     *
     *  \return string
     */
    /****************************************************************************/
    QString GetProgramID() const { return m_ProgramID; }

    /****************************************************************************/
    /*!
     *  \brief  Set the Rack program ID
     *
     *  \iparam ProgramID = Program ID value
     */
    /****************************************************************************/
    void SetProgramID(const QString ProgramID) { m_ProgramID = ProgramID; }

    /****************************************************************************/
    /*!
     *  \brief  Get the Rack to be transported to Sepia or not
     *
     *  \return bool
     */
    /****************************************************************************/
    bool GetTransported() const { return m_Transported; }

    /****************************************************************************/
    /*!
     *  \brief  Set the Rack transfer flag
     *
     *  \iparam Transported = Rack transfer flag
     */
    /****************************************************************************/
    void SetTransported(const bool Transported) { m_Transported = Transported; }

    /****************************************************************************/
    /*!
     *  \brief  Get the whether it is belongs to Sepia station or not
     *
     *  \return bool
     */
    /****************************************************************************/
    bool GetSepiaStation() const { return m_SepiaStation; }

    /****************************************************************************/
    /*!
     *  \brief  Set the Sepia station
     *
     *  \iparam SepiaStation = Sepia station flag
     */
    /****************************************************************************/
    void SetSepiaStation(const bool SepiaStation) { m_SepiaStation = SepiaStation; }

    /****************************************************************************/
    /*!
     *  \brief  Get the whether it is relevant for Sepia
     *
     *  \return bool
     */
    /****************************************************************************/
    bool GetUseSepia() const { return m_UseSepia; }

    /****************************************************************************/
    /*!
     *  \brief  Set the Sepia relevance
     *
     *  \iparam UseSepia = Usage within Sepia
     */
    /****************************************************************************/
    void SetUseSepia(const bool UseSepia) { m_UseSepia = UseSepia; }

    /****************************************************************************/
    /*!
     *  \brief  Get the start time of the Rack
     *
     *  \return Datetime
     */
    /****************************************************************************/
    QDateTime GetStartTime() const { return m_StartTime; }

    /****************************************************************************/
    /*!
     *  \brief  Set the start time of the Rack
     *
     *  \iparam StartTime = Start time value
     */
    /****************************************************************************/
    void SetStartTime(const QDateTime StartTime) { m_StartTime = StartTime; }

    /****************************************************************************/
    /*!
     *  \brief  Get the Colorado end time of the Rack
     *
     *  \return DateTime
     */
    /****************************************************************************/
    QDateTime GetEndTimeColorado() const { return m_EndTimeColorado; }

    /****************************************************************************/
    /*!
     *  \brief  Set the Colorado end time of the Rack
     *
     *  \iparam EndTimeColorado = End time of Colorado value
     */
    /****************************************************************************/
    void SetEndTimeColorado(const QDateTime EndTimeColorado) { m_EndTimeColorado = EndTimeColorado; }

    /****************************************************************************/
    /*!
     *  \brief  Get the Rack station ID
     *
     *  \return string
     */
    /****************************************************************************/
    QString GetStationID() const { return m_StationID; }

    /****************************************************************************/
    /*!
     *  \brief  Set the Rack station ID
     *
     *  \iparam StationID = Station ID value
     */
    /****************************************************************************/
    void SetStationID(const QString StationID) { m_StationID = StationID; }

    /****************************************************************************/
    /*!
     *  \brief  Get the Rack program step index
     *
     *  \return unsigned integer
     */
    /****************************************************************************/
    quint16 GetProgramStepIndex() const { return m_ProgramStepIndex; }

    /****************************************************************************/
    /*!
     *  \brief  Set the Rack program step index
     *
     *  \iparam ProgramStepIndex = Program step index value
     */
    /****************************************************************************/
    void SetProgramStepIndex(const quint16 ProgramStepIndex) { m_ProgramStepIndex = ProgramStepIndex; }

    /****************************************************************************/
    /*!
     *  \brief  Get the station end time of the Rack
     *
     *  \return Datetime
     */
    /****************************************************************************/
    QDateTime GetEndTimeStation() const { return m_EndTimeStation; }

    /****************************************************************************/
    /*!
     *  \brief  Set the station end time of the Rack
     *
     *  \iparam EndTimeStation = End time of the station
     */
    /****************************************************************************/
    void SetEndTimeStation(const QDateTime EndTimeStation) { m_EndTimeStation = EndTimeStation; }

    /****************************************************************************/
    /*!
     *  \brief  Get the color of the Rack
     *
     *  \return string
     */
    /****************************************************************************/
    QString GetColor() const { return m_Color; }

    /****************************************************************************/
    /*!
     *  \brief  Get the Sepia end time of the Rack
     *
     *  \return Datetime
     */
    /****************************************************************************/
    QDateTime GetEndTimeSepia() const { return m_EndTimeSepia; }

    /****************************************************************************/
    /*!
     *  \brief  Set the Sepia end time of the Rack
     *
     *  \iparam EndTimeSepia = End time of Sepia
     */
    /****************************************************************************/
    void SetEndTimeSepia(const QDateTime EndTimeSepia) { m_EndTimeSepia = EndTimeSepia; }

    /****************************************************************************/
    /*!
     *  \brief  Get the orientation of the Rack
     *
     *  \return Successful (true) or not (false)
     */
    /****************************************************************************/
    bool GetOrientation() const { return m_Orientation; }

    /****************************************************************************/
    /*!
     *  \brief  Set Actual slides ,this value is provided by scheduler when the
     *          rack is placed in the station
     *
     *  \iparam ActualSlides = actual slides of the rack
     */
    /****************************************************************************/
    void SetActualSlides(quint32 ActualSlides) { m_ActualSlides = ActualSlides; }
    /****************************************************************************/
    /*!
     *  \brief  Get actual number of slides
     *
     *  \return ActualSlides
     */
    /****************************************************************************/
    quint32 GetActualSlides() const { return m_ActualSlides;}

    /****************************************************************************/
    /*!
     *  \brief  Set Actual slide forecast ,this value is provided by scheduler when the
     *          rack is placed in the station
     *
     *  \iparam ActualSlideForecastValue = actual slides forecast value
     */
    /****************************************************************************/
    void SetActualSlideForecast(quint32 ActualSlideForecastValue) { m_ActualSlideForecastValue = ActualSlideForecastValue; }
    /****************************************************************************/
    /*!
     *  \brief  Get actual number of slide forecast
     *
     *  \return forecast value
     */
    /****************************************************************************/
    quint32 GetActualSlideForecast() const { return m_ActualSlideForecastValue;}


    bool IsRack5() const;


private:
    // enum for rack colors
    enum
    {
        WHITE,
        YELLOW,
        LIGHT_BLUE,
        DARK_BLUE,
        PINK,
        RED,
        GREEN,
        BLACK,
        GREY,
    };

    // user cannot create the default constructor as it needs the RFID unique ID and RFID
    // user data to create a Rack
    CRack();

    bool SerializeContent (QXmlStreamWriter &XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader &XmlStreamReader, bool CompleteData);

    /****************************************************************************/
    /*!
     *  \brief  Set the Rack RFID unique ID
     *
     *  \iparam RFIDUniqueID = Rack RF ID unique ID value
     */
    /****************************************************************************/
    void SetRFIDUniqueID(const quint32 RFIDUniqueID) { m_RFIDUniqueID = RFIDUniqueID; }

    void SetRFIDUserData(const quint32 RFIDUserData);

    /****************************************************************************/
    /*!
     *  \brief  Set the color of the Rack
     *
     *  \iparam Color = Color value of the Rack
     */
    /****************************************************************************/
    void SetColor(const QString Color) { m_Color = Color; }

    /****************************************************************************/
    /*!
     *  \brief  Set the orientation of the Rack
     *
     *  \iparam Value = Orientation value of the Rack
     */
    /****************************************************************************/
    void SetOrientation(const bool Value) { m_Orientation = Value; }



};

} // namespace DataManager

#endif // DATAMANAGER_RACK_H
