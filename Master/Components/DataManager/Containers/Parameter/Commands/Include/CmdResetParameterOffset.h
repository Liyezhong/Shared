/****************************************************************************/
/*! \file CmdResetParameterOffset.h
 *
 *  \brief ResetParameterOffset command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-03-09
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

#ifndef MSGCLASSES_CMDRESETPARAMETEROFFSET_H
#define MSGCLASSES_CMDRESETPARAMETEROFFSET_H

#include "Global/Include/Commands/Command.h"

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdAddParameterOffset command.
 */
/****************************************************************************/
class CmdResetParameterOffset : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdResetParameterOffset &);
    friend QDataStream & operator >> (QDataStream &, CmdResetParameterOffset &);
public:
    static QString NAME;    ///< Command name.

/****************************************************************************/

    CmdResetParameterOffset(int Timeout,  QString ParameterKey);
    CmdResetParameterOffset();
    ~CmdResetParameterOffset();
    virtual QString GetName() const;
    QString  GetParameterKey();
private:
    CmdResetParameterOffset(const CmdResetParameterOffset &);                       ///< Not implemented.
    const CmdResetParameterOffset & operator = (const CmdResetParameterOffset &);   ///< Not implemented.
    QString m_ParameterKey;                                 ///< Byte Array containing Rack ID
}; // end class CmdResetParameterOffset


/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdResetParameterOffset &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_ParameterKey;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdResetParameterOffset &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ParameterKey;
    return Stream;
}

} // end namespace MsgClasses

#endif // MSGCLASSES_CMDRESETPARAMETEROFFSET_H
