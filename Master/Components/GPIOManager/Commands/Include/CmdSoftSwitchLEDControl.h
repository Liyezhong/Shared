/****************************************************************************/
/*! \file CmdSoftSwitchLEDControl.h
 *
 *  \brief Command to control LED's on softswitch.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-02-19
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
#ifndef CMDSOFTSWITCHLEDCONTROL_H
#define CMDSOFTSWITCHLEDCONTROL_H
#include <Global/Include/Commands/Command.h>

//lint -sem(GPIOManager::CmdSoftSwitchLEDControl::CopyFrom,initializer)

namespace GPIOManager {

//!< Enumeration of LED control
typedef enum {
    GREEN_LED_ON,
    GREEN_LED_OFF,
    RED_LED_ON,
    RED_LED_OFF,
    GREEN_RED_LED_ON,
    GREEN_RED_LED_OFF
}LEDControl_t;

/****************************************************************************/
/*!
 * \brief Command to control LCD power
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdSoftSwitchLEDControl : public Global::Command {
private:
     static int  m_CmdTimeout;   ///< The allowed timeout for this command
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    void CopyFrom(const CmdSoftSwitchLEDControl &rOther);
protected:
public:
    static QString  NAME;                   ///< Command name.
    LEDControl_t    m_LEDControl;           ///< LED control type.

    /****************************************************************************/
    /**
     * \brief Constructor.
     * \iparam LEDControl = \ref LEDControl_t
     */
    /****************************************************************************/
    CmdSoftSwitchLEDControl(LEDControl_t LEDControl);
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    CmdSoftSwitchLEDControl(const CmdSoftSwitchLEDControl &rOther);
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \param[in]   rOther  Instance to copy from.
     * \return              Const reference to self.
     */
    /****************************************************************************/
    const CmdSoftSwitchLEDControl & operator = (const CmdSoftSwitchLEDControl &rOther);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdSoftSwitchLEDControl();
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
}; // end class CmdSoftSwitchLEDControl

}// end of namespace GPIOManager
#endif // CMDSOFTSWITCHLEDCONTROL_H
