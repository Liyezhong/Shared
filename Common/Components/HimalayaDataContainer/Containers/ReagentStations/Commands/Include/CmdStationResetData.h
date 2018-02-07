/****************************************************************************/
/*! \file CmdStationResetData.h
 *
 *  \brief Reset station data command definition.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-02-02
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

#ifndef MSGCLASSES_CMDSTATIONRESETDATA_H
#define MSGCLASSES_CMDSTATIONRESETDATA_H

#include "Global/Include/Commands/Command.h"

namespace MsgClasses {


/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class CmdStationResetData
 */
/****************************************************************************/
class CmdStationResetData : public Global::Command
{
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator<<
     */
    /****************************************************************************/
    friend QDataStream & operator << (QDataStream &, const CmdStationResetData &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator>>
     */
    /****************************************************************************/
    friend QDataStream & operator >> (QDataStream &, CmdStationResetData &);

public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CmdStationResetData
     *
     *  \param timeOut = int type parameter
     *  \param stationID =  const QString type parameter
     *
     *  \return from CmdStationResetData
     */
    /****************************************************************************/
    CmdStationResetData(int timeOut, const QString& stationID);
    CmdStationResetData(void);

    ~CmdStationResetData(void);

    virtual QString GetName(void) const;

    static QString NAME;       ///<  Definition/Declaration of variable NAME
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function StationID
     *
     *  \return from StationID
     */
    /****************************************************************************/
    inline const QString StationID() const { return m_StationID; }
private:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CmdStationResetData
     *
     *  \return from CmdStationResetData
     */
    /****************************************************************************/
    CmdStationResetData(const CmdStationResetData &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function operator=
     *
     *  \return from operator=
     */
    /****************************************************************************/
    const CmdStationResetData &operator = (const CmdStationResetData &);
    QString m_StationID;       ///<  Definition/Declaration of variable m_StationID

}; // end class CmdStationResetData

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function operator<<
 *
 *  \return from operator<<
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdStationResetData &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_StationID;
    return Stream;
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function operator>>
 *
 *  \return from operator>>
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdStationResetData &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_StationID;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDSTATIONRESETDATA_H
