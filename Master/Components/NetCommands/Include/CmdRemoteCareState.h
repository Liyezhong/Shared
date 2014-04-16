/****************************************************************************/
/*! \file CmdRemoteCareState.h
 *
 *  \brief CmdRemoteCareState command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 13.03.2013
 *   $Author:  $ Bhanu Prasad H
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
#ifndef CMD_REMOTECARE_STATE_H
#define CMD_REMOTECARE_STATE_H
#include <Global/Include/Commands/Command.h>

namespace NetCommands {
/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRackRemove command.
 */
/****************************************************************************/
class CmdRemoteCareState : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdRemoteCareState &);
    friend QDataStream & operator >> (QDataStream &, CmdRemoteCareState &);
public:
    static QString NAME;    ///< Command name.
    CmdRemoteCareState(int TimeOut,const bool &RemoteCareState);
    CmdRemoteCareState();
    ~CmdRemoteCareState();
    virtual QString GetName() const;
    bool GetRemoteCareState() const;

private:
    CmdRemoteCareState(const CmdRemoteCareState &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdRemoteCareState & operator = (const CmdRemoteCareState &);   ///< Not implemented.
    bool m_RemoteCareState;   ///< RemoteCare State - True if RemoteCare running, else false
};


/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRemoteCareState &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_RemoteCareState;
    return Stream;
}

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdRemoteCareState &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_RemoteCareState;
    return Stream;
}

} // End of namespace NetCommands
#endif // CMD_REMOTECARE_STATE_H
