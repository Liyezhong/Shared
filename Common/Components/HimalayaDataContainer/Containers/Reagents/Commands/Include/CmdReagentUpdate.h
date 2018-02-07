/****************************************************************************/
/*! \file CmdReagentUpdate.h
 *
 *  \brief CmdReagentUpdate command definition.
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

#ifndef MSGCLASSES_CMDREAGENTUPDATE_H
#define MSGCLASSES_CMDREAGENTUPDATE_H

#include "Global/Include/Commands/Command.h"

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdReagentUpdate command.
 */
/****************************************************************************/
class CmdReagentUpdate : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdReagentUpdate &);
    friend QDataStream & operator >> (QDataStream &, CmdReagentUpdate &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdReagentUpdate(int Timeout, const QDataStream &ReagentDataStream);
    CmdReagentUpdate();
    ~CmdReagentUpdate();
    virtual QString GetName() const;
    /****************************************************************************/
    /**
     * \brief Retrieve reagent data
     * \return Reagent Data
     **/
    /****************************************************************************/
    QByteArray const & GetReagentData() const { return m_ReagentByteArray;}

private:
    CmdReagentUpdate(const CmdReagentUpdate &);                     ///< Not implemented.
    const CmdReagentUpdate & operator = (const CmdReagentUpdate &); ///< Not implemented.
    QByteArray m_ReagentByteArray;//!< Byte Array filled with reagent data
}; // end class CmdReagentUpdate

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdReagentUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_ReagentByteArray;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdReagentUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ReagentByteArray;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDREAGENTUPDATE_H
