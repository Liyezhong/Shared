/****************************************************************************/
/*! \file CmdRackUpdate.h
 *
 *  \brief CmdRackUpdate command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.01.2012
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

#ifndef MSGCLASSES_CMDRACKUPDATE_H
#define MSGCLASSES_CMDRACKUPDATE_H

#include "Global/Include/Commands/Command.h"

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRackUpdate command.
 */
/****************************************************************************/
class CmdRackUpdate : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdRackUpdate &);
    friend QDataStream & operator >> (QDataStream &, CmdRackUpdate &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdRackUpdate(int Timeout, const QDataStream &RackStream);
    CmdRackUpdate();
    ~CmdRackUpdate();
    virtual QString GetName() const;
    QByteArray const & GetRackData() const { return m_RackByteArray;}
private:
    CmdRackUpdate(const CmdRackUpdate &);                       ///< Not implemented.
    const CmdRackUpdate & operator = (const CmdRackUpdate &);   ///< Not implemented.
    QByteArray m_RackByteArray;                                 ///< Byte Array containing Rack ID
}; // end class CmdRackUpdate


/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRackUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_RackByteArray;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdRackUpdate &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_RackByteArray;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDRACKUPDATE_H
