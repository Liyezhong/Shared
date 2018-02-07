/****************************************************************************/
/*! \file CmdReagentRemove.h
 *
 *  \brief CmdReagentRemove command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 27.03.2011
 *   $Author:  $ M.Scherer
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

#ifndef MSGCLASSES_CMDREAGENTREMOVE_H
#define MSGCLASSES_CMDREAGENTREMOVE_H


#include "Global/Include/Commands/Command.h"

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdReagentRemove command.
 */
/****************************************************************************/
class CmdReagentRemove : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdReagentRemove &);
    friend QDataStream & operator >> (QDataStream &, CmdReagentRemove &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdReagentRemove(int Timeout, const QString &ReagentID);
    CmdReagentRemove();
    ~CmdReagentRemove();
    virtual QString GetName() const;
    QString GetReagentID() const;
private:
    CmdReagentRemove(const CmdReagentRemove &);                     ///< Not implemented.
    const CmdReagentRemove & operator = (const CmdReagentRemove &); ///< Not implemented.
    QString      m_ReagentID;      ///< Item ID.
}; // end class CmdReagentRemove

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdReagentRemove &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_ReagentID;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdReagentRemove &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ReagentID;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDREAGENTREMOVE_H
