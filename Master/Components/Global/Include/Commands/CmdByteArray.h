/****************************************************************************/
/*! \file Global/Include/Commands/CmdByteArray.h
 *
 *  \brief CmdByteArray command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.01.2012
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

#ifndef GLOBAL_CMDBYTEARRAY_H
#define GLOBAL_CMDBYTEARRAY_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/Commands/Acknowledge.h>

#include <QByteArray>

namespace Global {


/****************************************************************************/
/*!
 *  \brief  This class implements a CmdByteArray command.
 *
 *      The class can be used as a base class for all commands that transfer
 *      a single QByteArray in their data field.
 */
/****************************************************************************/

class CmdByteArray : public Command {
    friend class TestCommands;
friend QDataStream & operator << (QDataStream &, const CmdByteArray &);
friend QDataStream & operator >> (QDataStream &, CmdByteArray &);
public:
    CmdByteArray();
    CmdByteArray(int Timeout, const QByteArray &ba);
    CmdByteArray(const CmdByteArray &rOther);
    ~CmdByteArray();
    const CmdByteArray & operator = (const CmdByteArray &rOther);

    QByteArray const& GetByteArray() const;

private:
    void CopyFrom(const CmdByteArray &rOther);

private:
    QByteArray m_ByteArray; //!< The data field of the command

}; // end class CmdByteArray

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdByteArray &Cmd) {
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_ByteArray;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdByteArray &Cmd) {
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ByteArray;
    return Stream;
}

} // end namespace Global

#endif
