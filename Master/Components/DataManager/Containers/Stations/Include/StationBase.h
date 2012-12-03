/****************************************************************************/
/*! \file StationBase.h
 *
 *  \brief Definition for CStationBase class.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-11-29
 *   $Author:  $ Michael Thiel
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

#ifndef DATAMANAGER_STATIONBASE_H
#define DATAMANAGER_STATIONBASE_H

#include <QString>
#include <QDate>
#include <QXmlStreamReader>

#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Helper/Include/Helper.h"
#include "Global/Include/GlobalDefines.h"

namespace DataManager {

const char LEICA_REAGENT_START_CHARACTER = 'L';     //!< Leica reagent start character
const char SPECIAL_REAGENT_START_CHARACTER = 'S';   //!< Leica reagent start character

/****************************************************************************/
/*!
 *  \brief CStationBase class containing the reagents attributes
 */
/****************************************************************************/
class CStationBase
{
    friend class CDataStationList;
private:
    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);

protected:
    QString m_StationID;                        //!< Station ID
    int m_StationTemp;                          //!< Station Temperature (current temperature)
    Global::HeatingState m_HeatingState;        //!< HeatingState of Heated Cuvette/Oven.
    bool m_Defect;                              //!< Whether the station is defect or not

public:
    CStationBase();
    CStationBase(const QString ID);
    CStationBase(const CStationBase&);
    void CopyFromOther(const CStationBase &Station);
    friend QDataStream& operator <<(QDataStream& OutDataStream, const CStationBase& Station);
    friend QDataStream& operator >>(QDataStream& InDataStream, CStationBase& Station);
    CStationBase& operator=(const CStationBase&);

    StationsType_t GetStationType();

    /******************** INLINE FuNCTIONS **************************************/
    /****************************************************************************/
    /*!
     *  \brief Get's the Station ID
     *
     *  \return Station Id String
     */
    /****************************************************************************/
    QString  GetStationID() {return m_StationID;}

    /****************************************************************************/
    /*!
     *  \brief Set's the Station ID
     *
     *  \iparam Value = station ID string
     *
     *  \return
     */
    /****************************************************************************/
    void SetStationID(const QString Value){m_StationID = Value.trimmed();}
    

    /****************************************************************************/
    /*!
     *  \brief Gets the Station Temperature of reagent
     *
     *  \return Station Temperature
     */
    /****************************************************************************/
    int GetStationTemperature() { return m_StationTemp; }

    /****************************************************************************/
    /*!
     *  \brief Sets the Station Temperature
     *
     *  \iparam Temp = Station Temperature
     *
     *  \return
     */
    /****************************************************************************/
    void SetStationTemperature(const int Temp) {m_StationTemp =  Temp;}


    /****************************************************************************/
    /*!
     *  \brief Sets the heating state of heated cuvette/oven station
     *
     *  \iparam HeatingState = on/Temperature in range/out of range
     */
    /****************************************************************************/
    void SetHeatingState(Global::HeatingState HeatingState) {m_HeatingState = HeatingState;}

    /****************************************************************************/
    /*!
     *  \brief Retrieves the Heating state of heated cuvette/oven station
     *
     *  \return Heating state
     */
    /****************************************************************************/
    Global::HeatingState GetHeatingState() {return m_HeatingState;}

    /****************************************************************************/
    /*!
     *  \brief Gets the Station Defect State
     *
     *  \return true or false
     */
    /****************************************************************************/
    bool IsStationDefect() { return m_Defect; }

    /****************************************************************************/
    /*!
     *  \brief Sets the Station Defect State
     *
     *  \iparam Defect = True or false, bool type
     *
     *  \return
     */
    /****************************************************************************/
    void MarkStationDefect(bool Defect) { m_Defect = Defect; }
};

} // namespace DataManager

#endif // DATAMANAGER_STATIONBASE_H
