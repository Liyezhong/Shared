/****************************************************************************/
/*! \file CmdExecutionStateChanged.h
 *
 *  \brief Definition file for class CmdExecutionStateChanged.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-08-13
 *  $Author:    $ N.Kamath
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef CMDEXECUTIONSTATECHANGED_H
#define CMDEXECUTIONSTATECHANGED_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>

namespace NetCommands {

/****************************************************************************/
/**
 * \brief \todo comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdExecutionStateChanged : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdExecutionStateChanged &);
    friend QDataStream & operator >> (QDataStream &, CmdExecutionStateChanged &);
private:
    /****************************************************************************/

    CmdExecutionStateChanged(const CmdExecutionStateChanged &);                     ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdExecutionStateChanged & operator = (const CmdExecutionStateChanged &); ///< Not implemented.
protected:
public:
    static QString  NAME;           ///< Command name.
    bool m_Stop;                    ///< True - if execution is stopped
    bool m_WaitDialogFlag;            ///< True - Wait dialog is displayed
    Global::WaitDialogText_t m_WaitDialogText; ///< Wait dialog message text
    /****************************************************************************/
    /*!
     * \brief   Constructor
     */
    /****************************************************************************/
    CmdExecutionStateChanged();
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   TimeOut       Timeout for Command
     */
    /****************************************************************************/
    CmdExecutionStateChanged(int TimeOut);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdExecutionStateChanged();
    /****************************************************************************/
    /**
     * \brief Get command name.
     *
     * \return  Command name.
     */
    /****************************************************************************/
    virtual QString GetName() const {
        return NAME;
    }
}; // end class CmdExecutionStateChanged

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdExecutionStateChanged &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_Stop << Cmd.m_WaitDialogFlag << static_cast<int>(Cmd.m_WaitDialogText);
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
inline QDataStream & operator >> (QDataStream &Stream, CmdExecutionStateChanged &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_Stop;
    Stream >> Cmd.m_WaitDialogFlag;
    int WaitDlgText;
    Stream >> WaitDlgText;
    Cmd.m_WaitDialogText = (Global::WaitDialogText_t) (WaitDlgText);
    return Stream;
}
} // end namespace NetCommands

#endif // CMDEXECUTIONSTATECHANGED_H
