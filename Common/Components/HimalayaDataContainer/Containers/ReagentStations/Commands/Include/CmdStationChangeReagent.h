/****************************************************************************/
/*! \file CmdStationChangeReagent.h
 *
 *  \brief Change reagent in station command definition.
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

#ifndef MSGCLASSES_CMDSTATIONCHANGEREAGENT_H
#define MSGCLASSES_CMDSTATIONCHANGEREAGENT_H

#include "Global/Include/Commands/Command.h"

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class CmdStationChangeReagent
 */
/****************************************************************************/
class CmdStationChangeReagent : public Global::Command
{
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator<<
     */
    /****************************************************************************/
    friend QDataStream & operator << (QDataStream &, const CmdStationChangeReagent &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator>>
     */
    /****************************************************************************/
    friend QDataStream & operator >> (QDataStream &, CmdStationChangeReagent &);

public:
    CmdStationChangeReagent(int timeOut, const QString& stationID, const QString& reagentID);
    CmdStationChangeReagent(void);

    ~CmdStationChangeReagent(void);

    virtual QString GetName(void) const;

    static QString NAME; ///< Command name.
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function StationID
     *
     *  \return from StationID
     */
    /****************************************************************************/
    const QString& StationID()const;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ReagentID
     *
     *  \return from ReagentID
     */
    /****************************************************************************/
    const QString& ReagentID()const;

private:
    CmdStationChangeReagent(const CmdStationChangeReagent &); ///< Not implemented.
    const CmdStationChangeReagent &operator = (const CmdStationChangeReagent &); ///< Not implemented.

    QString m_StationID;       ///<  Definition/Declaration of variable m_StationID
    QString m_ReagentID;       ///<  Definition/Declaration of variable m_ReagentID
}; // end class CmdStationChangeReagent

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function operator<<
 *
 *  \return from operator<<
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdStationChangeReagent &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_StationID;
    Stream << Cmd.m_ReagentID;
    return Stream;
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function operator>>
 *
 *  \return from operator>>
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdStationChangeReagent &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_StationID;
    Stream >> Cmd.m_ReagentID;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDSTATIONCHANGEREAGENT_H
