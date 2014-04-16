/****************************************************************************/
/*! \file CmdProcessState.h
 *
 *  \brief CmdProcessState command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 03.04.2012
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
#ifndef CMDPROCESSSTATE_H
#define CMDPROCESSSTATE_H
#include <Global/Include/Commands/Command.h>

namespace NetCommands {
/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRackRemove command.
 */
/****************************************************************************/
class CmdProcessState : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdProcessState &);
    friend QDataStream & operator >> (QDataStream &, CmdProcessState &);
public:
    static QString NAME;    ///< Command name.
    CmdProcessState(int TimeOut,const bool &ProcessState);
    CmdProcessState();
    ~CmdProcessState();
    virtual QString GetName() const;
    bool GetProcessState() const;

private:
    CmdProcessState(const CmdProcessState &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdProcessState & operator = (const CmdProcessState &);   ///< Not implemented.
    bool m_ProcessState;   ///< Process State - True if process running, else false
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
inline QDataStream & operator << (QDataStream &Stream, const CmdProcessState &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_ProcessState;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdProcessState &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ProcessState;
    return Stream;
}

} // End of namespace NetCommands
#endif // CMDPROCESSSTATE_H
