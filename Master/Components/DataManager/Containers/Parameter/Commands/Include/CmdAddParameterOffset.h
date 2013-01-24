/****************************************************************************/
/*! \file CmdAddParameterOffset.h
 *
 *  \brief CmdAddParameterOffset command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-09-01
 *   $Author:  $ Thirumalesha R
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

#ifndef MSGCLASSES_CMDADDPARAMETEROFFSET_H
#define MSGCLASSES_CMDADDPARAMETEROFFSET_H

#include "Global/Include/Commands/Command.h"

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdAddParameterOffset command.
 */
/****************************************************************************/
class CmdAddParameterOffset : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdAddParameterOffset &);
    friend QDataStream & operator >> (QDataStream &, CmdAddParameterOffset &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdAddParameterOffset(int Timeout, const QByteArray &ParameterOffset);
    CmdAddParameterOffset();
    ~CmdAddParameterOffset();
    virtual QString GetName() const;
    QByteArray const & GetParameterOffset() const { return m_ParameterOffset;}
private:
    CmdAddParameterOffset(const CmdAddParameterOffset &);                       ///< Not implemented.
    const CmdAddParameterOffset & operator = (const CmdAddParameterOffset &);   ///< Not implemented.
    QByteArray m_ParameterOffset;                                 ///< Byte Array containing Rack ID
}; // end class CmdAddParameterOffset


/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdAddParameterOffset &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_ParameterOffset;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdAddParameterOffset &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ParameterOffset;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDADDPARAMETEROFFSET_H
