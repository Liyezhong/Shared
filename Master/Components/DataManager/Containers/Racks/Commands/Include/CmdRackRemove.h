/****************************************************************************/
/*! \file CmdRackRemove.h
 *
 *  \brief CmdRackRemove command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16.03.2012
 *   $Author:  $ N.Kamath
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

#ifndef MSGCLASSES_CMDRACKREMOVE_H
#define MSGCLASSES_CMDRACKREMOVE_H
#include <Global/Include/Commands/Command.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRackRemove command.
 */
/****************************************************************************/
class CmdRackRemoved : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdRackRemoved &);
    friend QDataStream & operator >> (QDataStream &, CmdRackRemoved &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdRackRemoved(int Timeout, const quint32 &RackRfid);
    CmdRackRemoved();                                            ///< Not implemented.
    ~CmdRackRemoved();
    virtual QString GetName() const;
    quint32 GetRackRfid() const;

private:
    CmdRackRemoved(const CmdRackRemoved &);                       ///< Not implemented.
    const CmdRackRemoved & operator = (const CmdRackRemoved &);   ///< Not implemented.
    quint32 m_RackRfid;   ///< Program ID.
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
inline QDataStream & operator << (QDataStream &Stream, const CmdRackRemoved &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_RackRfid;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdRackRemoved &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_RackRfid;
    return Stream;
}


}//End of namespace MsgClasses
#endif // MSGCLASSES_CMDRACKREMOVE_H
