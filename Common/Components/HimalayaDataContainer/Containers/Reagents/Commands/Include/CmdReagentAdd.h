/****************************************************************************/
/*! \file CmdReagentAdd.h
 *
 *  \brief CmdReagentAdd command definition.
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

#ifndef MSGCLASSES_CMDREAGENTADD_H
#define MSGCLASSES_CMDREAGENTADD_H


#include "Global/Include/Commands/Command.h"

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdReagentAdd command.
 */
/****************************************************************************/
class CmdReagentAdd : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdReagentAdd &);
    friend QDataStream & operator >> (QDataStream &, CmdReagentAdd &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdReagentAdd(int Timeout, const QDataStream &ReagentData);
    CmdReagentAdd();
    ~CmdReagentAdd();
    virtual QString GetName() const;
    /****************************************************************************/
    /**
     * \brief Retrieve reagent data
     * \return Reagent Data
     **/
    /****************************************************************************/
    QByteArray const & GetReagentData() const { return m_ReagentByteArray;}

private:
    CmdReagentAdd(const CmdReagentAdd &);                     ///< Not implemented.
    const CmdReagentAdd & operator = (const CmdReagentAdd &); ///< Not implemented.    
    QByteArray m_ReagentByteArray;                            ///< ByteArray containing Reagent data.
}; // end class CmdReagentAdd

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdReagentAdd &Cmd)
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
inline QDataStream & operator >> (QDataStream &Stream, CmdReagentAdd &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ReagentByteArray;
    return Stream;
}
} // end namespace MsgClasses

#endif // MSGCLASSES_CMDREAGENTADD_H
