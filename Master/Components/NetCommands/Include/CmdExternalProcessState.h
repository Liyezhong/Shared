/****************************************************************************/
/*! \file CmdExternalProcessState.h
 *
 *  \brief CmdExternalProcessState command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-11-05
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
#ifndef CMDEXTERNALPROCESSSTATE_H
#define CMDEXTERNALPROCESSSTATE_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/**
 * \brief Enum for External Process type.
 */
/****************************************************************************/
enum ProcessType_t{
    HIMALAYA_GUI_PROCESS,
    SEPIA_MAIN_PROCESS
};

/****************************************************************************/
/**
 * \brief \todo comment and implement
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdExternalProcessState : public Global::Command {
private:
    /****************************************************************************/
    CmdExternalProcessState(const CmdExternalProcessState &);                     ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdExternalProcessState & operator = (const CmdExternalProcessState &); ///< Not implemented.

protected:
public:
    static QString  NAME;           ///< Command name.

    ProcessType_t m_ProcessType;    ///< External Process type
    bool m_Connected;               ///< True - if external process connected, else false.
    bool m_StoppedForEver;          ///< True - External Process is stopped for ever.

    /****************************************************************************/
    /*!
     * \brief   Constructor
     */
    /****************************************************************************/
    CmdExternalProcessState();
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   TimeOut       Timeout for Command.
     * \iparam   ProcessType   External Process type
     * \iparam   Connected     True - if external process connected, else false.
     * \iparam      StoppedForEver True - if external process is stopped, else false.
     */
    /****************************************************************************/
    CmdExternalProcessState(int TimeOut, ProcessType_t ProcessType, bool Connected, bool StoppedForEver = false);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdExternalProcessState();
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
}; // end class CmdExternalProcessState

} // end namespace NetCommands

#endif // CMDEXTERNALPROCESSSTATE_H
