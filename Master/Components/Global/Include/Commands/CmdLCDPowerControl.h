/****************************************************************************/
/*! \file Global/Include/Commands/CmdLCDPowerControl.h
 *
 *  \brief Definition file for class CmdLCDPowerControl.
 *         This command is sent from main software to turn ON/OFF the LCD.
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
#ifndef CMDLCDPOWERCONTROL_H
#define CMDLCDPOWERCONTROL_H
#include <Global/Include/Commands/Command.h>

//lint -sem(Global::CmdLCDPowerControl::CopyFrom,initializer)
namespace Global {

/****************************************************************************/
/**
 * \brief Command to control LCD power
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdLCDPowerControl : public Command {
    friend class TestCommands;
private:
     static int  m_CmdTimeout;   ///< The allowed timeout for this command
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    void CopyFrom(const CmdLCDPowerControl &rOther);
protected:
public:
    static QString  NAME;                   ///< Command name.
    bool m_LCDOn;                           ///< true for turning the LCD ON, false for OFF
    /****************************************************************************/
    /**
     * \brief Constructor.
     * \iparam LCDActive = true for turning ON LCD, false for turning OFF.
     */
    /****************************************************************************/
    CmdLCDPowerControl(bool LCDActive);
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    CmdLCDPowerControl(const CmdLCDPowerControl &rOther);
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \param[in]   rOther  Instance to copy from.
     * \return              Const reference to self.
     */
    /****************************************************************************/
    const CmdLCDPowerControl & operator = (const CmdLCDPowerControl &rOther);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdLCDPowerControl();
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
}; // end class CmdLCDPowerControl
}// end of namespace global
#endif // CMDLCDPOWERCONTROL_H
