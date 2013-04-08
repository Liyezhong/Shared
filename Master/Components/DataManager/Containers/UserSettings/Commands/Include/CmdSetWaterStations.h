/****************************************************************************/
/*! \file Components/DataManager/Containers/UserSettings/Commands/Include/CmdSetWaterStations.h
 *
 *  \brief CmdSetWaterStations command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17.03.2011
 *   $Author:  $ Y.Novak
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

#ifndef MSGCLASSES_CMDSETWATERSTATIONS_H
#define MSGCLASSES_CMDSETWATERSTATIONS_H

#include "Global/Include/Commands/Command.h"

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdSetWaterStations command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdSetWaterStations : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdSetWaterStations &);
    friend QDataStream & operator >> (QDataStream &, CmdSetWaterStations &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdSetWaterStations(int Timeout, const QString &ReagentId, const QString &Group);
    ~CmdSetWaterStations();
    virtual QString GetName() const;
    QString GetReagentID() const;
    QString GetGroup() const;
private:
    CmdSetWaterStations();                                                  ///< Not implemented.
    CmdSetWaterStations(const CmdSetWaterStations &);                       ///< Not implemented.
    const CmdSetWaterStations & operator = (const CmdSetWaterStations &);   ///< Not implemented.
private:
    QString      m_ReagentId;       ///< Reagent ID.
    QString      m_Group;           ///< Group.
}; // end class CmdSetWaterStations

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdSetWaterStations &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_ReagentId << Cmd.m_Group;
    return Stream;
}

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdSetWaterStations &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ReagentId >> Cmd.m_Group;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDSETWATERSTATIONS_H
