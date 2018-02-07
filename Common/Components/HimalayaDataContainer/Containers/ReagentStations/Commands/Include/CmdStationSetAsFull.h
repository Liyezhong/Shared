/****************************************************************************/
/*! \file CmdStationSetAsFull.h
 *
 *  \brief Set the station as full command definition.
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

#ifndef MSGCLASSES_CMDSTATIONSETASFULL_H
#define MSGCLASSES_CMDSTATIONSETASFULL_H

#include "Global/Include/Commands/Command.h"

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class CmdStationSetAsFull
 */
/****************************************************************************/
class CmdStationSetAsFull : public Global::Command
{
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator<<
     */
    /****************************************************************************/
    friend QDataStream & operator << (QDataStream &, const CmdStationSetAsFull &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator>>
     */
    /****************************************************************************/
    friend QDataStream & operator >> (QDataStream &, CmdStationSetAsFull &);
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CmdStationSetAsFull
     *
     *  \param timeOut = int type parameter
     *  \param stationID =  const QString type parameter
     *
     *  \return from CmdStationSetAsFull
     */
    /****************************************************************************/
    CmdStationSetAsFull(int timeOut, const QString& stationID);
    CmdStationSetAsFull(void);

    ~CmdStationSetAsFull(void);

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
     *  \brief  Definition/Declaration of function CmdStationSetAsFull
     *
     *  \return from CmdStationSetAsFull
     */
    /****************************************************************************/
    CmdStationSetAsFull(const CmdStationSetAsFull &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function operator=
     *
     *  \return from operator=
     */
    /****************************************************************************/
    const CmdStationSetAsFull &operator = (const CmdStationSetAsFull &);
    QString m_StationID;       ///<  Definition/Declaration of variable m_StationID
}; // end class CmdStationSetAsFull

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function operator<<
 *
 *  \return from operator<<
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdStationSetAsFull &Cmd)
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
inline QDataStream & operator >> (QDataStream &Stream, CmdStationSetAsFull &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_StationID;
    return Stream;
}
} // end namespace MsgClasses

#endif // MSGCLASSES_CMDSTATIONSETASFULL_H
