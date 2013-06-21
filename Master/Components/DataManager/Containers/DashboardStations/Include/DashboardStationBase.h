/****************************************************************************/
/*! \file DashboardStationBase.h
 *
 *  \brief Definition for CDashboardStationBase class.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16-Jan-2013
 *   $Author:  $ Santhosha Kumar Raju
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2012 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef DATAMANAGER_DASHBOARDSTATIONBASE_H
#define DATAMANAGER_DASHBOARDSTATIONBASE_H

#include <QString>
#include <QDate>
#include <QXmlStreamReader>

#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Helper/Include/Helper.h"
#include "Global/Include/GlobalDefines.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief CDashboardStationBase class containing the reagents attributes
 */
/****************************************************************************/
class CDashboardStationBase
{
    friend class CDashboardDataStationList;
private:
    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);

protected:
    QString m_DashboardStationID;               //!< Station ID
    bool m_Defect;                              //!< Whether the station is defect or not
    bool m_Disabled;                            //!< Whether the station is disabled or not

public:
    CDashboardStationBase();
    CDashboardStationBase(const QString ID);
    CDashboardStationBase(const CDashboardStationBase&);
    void CopyFromOther(const CDashboardStationBase &Station);
    friend QDataStream& operator <<(QDataStream& OutDataStream, const CDashboardStationBase& Station);
    friend QDataStream& operator >>(QDataStream& InDataStream, CDashboardStationBase& Station);
    CDashboardStationBase& operator=(const CDashboardStationBase&);

    DashboardStationType_t GetDashboardStationType();

    /******************** INLINE FuNCTIONS **************************************/
    /****************************************************************************/
    /*!
     *  \brief Get's the Station ID
     *
     *  \return Station Id String
     */
    /****************************************************************************/
    QString  GetDashboardStationID() {return m_DashboardStationID;}

    /****************************************************************************/
    /*!
     *  \brief Set's the Station ID
     *
     *  \iparam Value = station ID string
     *
     *  \return
     */
    /****************************************************************************/
    void SetDashboardStationID(const QString Value){m_DashboardStationID = Value.trimmed();}
    
    /****************************************************************************/
    /*!
     *  \brief Gets the Station Defect State
     *
     *  \return true or false
     */
    /****************************************************************************/
    bool IsDashboardStationDefect() { return m_Defect; }

    /****************************************************************************/
    /*!
     *  \brief Sets the Station Defect State
     *
     *  \iparam Defect = True or false, bool type
     *
     *  \return
     */
    /****************************************************************************/
    void SetDashboardStationDefect(bool Defect) { m_Defect = Defect; }

    /****************************************************************************/
    /*!
     *  \brief Gets the Station Defect State
     *
     *  \return true or false
     */
    /****************************************************************************/
    bool IsDashboardStationDisabled() { return m_Disabled; }

    /****************************************************************************/
    /*!
     *  \brief Sets the Station Defect State
     *
     *  \iparam Defect = True or false, bool type
     *
     *  \return
     */
    /****************************************************************************/
    void SetDashboardStationDisabled(bool Disabled) { m_Disabled = Disabled; }

};

} // namespace DataManager

#endif // DATAMANAGER_DASHBOARDSTATIONBASE_H
