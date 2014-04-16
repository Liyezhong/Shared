/****************************************************************************/
/*! \file Global/Include/Commands/CmdSoftSwitchPressed.h
 *
 *  \brief Definition file for class CmdSoftSwitchPressed.
 *         This command is sent from GPIO ThreadController
 *         When user presses the softswitch to shutdown the
 *         Software.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-02-19
 *  $Author:    $ N.Kamath
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
#ifndef CMDSOFTSWITCHPRESSED_H
#define CMDSOFTSWITCHPRESSED_H
#include <Global/Include/Commands/Command.h>
#include <DeviceControl/Include/Global/DeviceControlGlobal.h>

namespace Global {

/****************************************************************************/
/**
 * \brief Command Created when softswitch is pressed
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdSoftSwitchPressed : public Global::Command {
private:
    static int  m_CmdTimeout;   ///< The allowed timeout for this command.
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CmdSoftSwitchPressed)
protected:
public:
    static QString  NAME;       ///< Command name.
    bool   PressedAtStartUp;    //!< True - if softswitch Pressed at startup

    CmdSoftSwitchPressed();

    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdSoftSwitchPressed();
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
}; // end class CmdSoftSwitchPressed

} // end namespace Global
#endif // CMDSOFTSWITCHPRESSED_H
