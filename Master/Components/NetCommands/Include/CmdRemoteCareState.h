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
 *  \brief  This class implements a CmdRemoteCareState command.
 */
/****************************************************************************/
class CmdRemoteCareState : public Global::Command {
    friend QDataStream & operator << (const QDataStream &, const CmdRemoteCareState &);
    friend QDataStream & operator >> (const QDataStream &, const CmdRemoteCareState &);
public:
    static QString NAME;    //!< Command name.
    CmdRemoteCareState(int TimeOut, const bool &RemoteCareState, QString ConnectionType = "RemoteCare");
    CmdRemoteCareState();
    ~CmdRemoteCareState();
    virtual QString GetName() const;
    bool GetRemoteCareState() const;
    QString GetConnectionType() const;

private:
    CmdRemoteCareState(const CmdRemoteCareState &);                       //!< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdRemoteCareState & operator = (const CmdRemoteCareState &);   //!< Not implemented.
    bool m_AgentState;   //!< RemoteCare State - True if RemoteCare running, else false
    QString m_ConnectionType;   //!< "RemoteCare", "LeicaApps"
};


/****************************************************************************/
/**
 * \brief Output Stream Operator which streams data
 *
 * \oparam   Stream      Stream to stream into.
 * \iparam   Cmd         The command to stream.
 * \return   Output Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (const QDataStream &Stream, const CmdRemoteCareState &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    // copy base class data
    Cmd.CopyToStream(StreamRef);
    // copy internal data
    StreamRef << Cmd.m_AgentState;
    StreamRef << Cmd.m_ConnectionType;
    return StreamRef;
}

/****************************************************************************/
/**
 * \brief Input Stream Operator which streams data
 *
 * \oparam   Stream      Stream to stream from.
 * \iparam   Cmd         The command to stream.
 * \return   Input Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (const QDataStream &Stream, const CmdRemoteCareState &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    CmdRemoteCareState& CmdRef = const_cast<CmdRemoteCareState &>(Cmd);

    // copy base class data
    CmdRef.CopyFromStream(StreamRef);
    // copy internal data
    StreamRef >> CmdRef.m_AgentState;
    StreamRef >> CmdRef.m_ConnectionType;
    return StreamRef;
}

} // End of namespace NetCommands
#endif // CMD_REMOTECARE_STATE_H
