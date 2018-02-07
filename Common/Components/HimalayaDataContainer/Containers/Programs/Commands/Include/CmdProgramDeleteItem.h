/****************************************************************************/
/*! \file CmdProgramDeleteItem.h
 *
 *  \brief CmdProgramDeleteItem command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.03.2011
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

#ifndef MSGCLASSES_CMDDELETEITEM_H
#define MSGCLASSES_CMDDELETEITEM_H

#include <Global/Include/Commands/Command.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdProgramDeleteItem command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdProgramDeleteItem : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdProgramDeleteItem &);
    friend QDataStream & operator >> (QDataStream &, CmdProgramDeleteItem &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdProgramDeleteItem(int Timeout, const QString &ItemId);
    CmdProgramDeleteItem();
    ~CmdProgramDeleteItem();
    virtual QString GetName() const;
    QString GetItemId() const;

private:
    CmdProgramDeleteItem(const CmdProgramDeleteItem &);                       ///< Not implemented.
    const CmdProgramDeleteItem & operator = (const CmdProgramDeleteItem &);   ///< Not implemented.
private:
    QString      m_ItemId;      ///< Item ID.
}; // end class CmdProgramDeleteItem

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdProgramDeleteItem &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_ItemId;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdProgramDeleteItem &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ItemId;
    return Stream;
}
} // end namespace MsgClasses

#endif // MSGCLASSES_CMDDELETEITEM_H
