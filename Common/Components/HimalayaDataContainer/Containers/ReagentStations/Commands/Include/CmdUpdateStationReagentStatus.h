/****************************************************************************/
/*! \file CmdUpdateStationReagentStatus.h
 *
 *  \brief when the running Program finishes with 1 Reagent, the Reagent status should be updated.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-03-20
 *  $Author:    $ Abe Yang
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2013 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef MSGCLASSES_CMDUPDATESTATIONREAGENTSTATUS_H
#define MSGCLASSES_CMDUPDATESTATIONREAGENTSTATUS_H

#include "Global/Include/Commands/Command.h"
#include <QStringList>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class CmdUpdateStationReagentStatus
 */
/****************************************************************************/
class CmdUpdateStationReagentStatus : public Global::Command
{
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator<<
     */
    /****************************************************************************/
    friend QDataStream & operator << (QDataStream &, const CmdUpdateStationReagentStatus &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of friend operator>>
     */
    /****************************************************************************/
    friend QDataStream & operator >> (QDataStream &, CmdUpdateStationReagentStatus &);

public:
    //if CassetteCount == 0, then bUpdateCycle
    CmdUpdateStationReagentStatus(int timeOut, const QStringList& stationIDs, int cassetteCount);
    CmdUpdateStationReagentStatus(void);

    ~CmdUpdateStationReagentStatus(void);

    virtual QString GetName(void) const;

    static QString NAME; ///< Command name.
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CassetteCount
     *
     *  \return from CassetteCount
     */
    /****************************************************************************/
    inline int CassetteCount()const{return m_CassetteCount;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function StationIDs
     *
     *  \return from StationIDs
     */
    /****************************************************************************/
    inline const QStringList& StationIDs() const {return m_StationIDs;}
    

private:
    CmdUpdateStationReagentStatus(const CmdUpdateStationReagentStatus &); ///< Not implemented.
    const CmdUpdateStationReagentStatus &operator = (const CmdUpdateStationReagentStatus &); ///< Not implemented.
    int m_CassetteCount;       ///<  Definition/Declaration of variable m_CassetteCount
    QStringList m_StationIDs;       ///<  Definition/Declaration of variable m_StationIDs
}; // end class CmdUpdateStationReagentStatus

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function operator<<
 *
 *  \return from operator<<
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdUpdateStationReagentStatus &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_StationIDs;
    Stream << Cmd.m_CassetteCount;
    return Stream;
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function operator>>
 *
 *  \return from operator>>
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdUpdateStationReagentStatus &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_StationIDs;
    Stream >> Cmd.m_CassetteCount;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDUPDATESTATIONREAGENTSTATUS_H
