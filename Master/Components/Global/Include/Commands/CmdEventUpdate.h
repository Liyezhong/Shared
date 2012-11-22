/****************************************************************************/
/*! \file CmdEventUpdate.h
 *
 *  \brief CmdEventUpdate command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.07.2012
 *   $Author:  $ M.Thiel
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#ifndef MSGCLASSES_CMDEVENTUPDATE_H
#define MSGCLASSES_CMDEVENTUPDATE_H

#include "Global/Include/Commands/Command.h"

namespace Global {
/****************************************************************************/
/*!
 *  \brief  This class implements a CmdEventUpdate command.
 */
/****************************************************************************/
class CmdEventUpdate : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdEventUpdate &);
    friend QDataStream & operator >> (QDataStream &, CmdEventUpdate &);
public:
    static QString NAME;    ///< Command name.
    CmdEventUpdate(quint32 eventID, bool active);
    CmdEventUpdate();
    ~CmdEventUpdate();
    virtual QString GetName() const;
//    quint32 getEventId() { return m_eventId; }
//    inline bool getActive() { return m_active; }
    quint32 m_eventId;
    bool m_active;

private:
    CmdEventUpdate(const CmdEventUpdate &);                       ///< Not implemented.
    const CmdEventUpdate & operator = (const CmdEventUpdate &);   ///< Not implemented.
};


/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdEventUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_eventId;
    Stream << Cmd.m_active;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdEventUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_eventId;
    Stream >> Cmd.m_active;
    return Stream;
}

} // End of namespace MsgClasses
#endif // MSGCLASSES_CMDEVENTUPDATE_H
