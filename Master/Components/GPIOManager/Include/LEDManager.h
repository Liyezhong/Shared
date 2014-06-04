/****************************************************************************/
/*! \file LEDManager.h
 *
 *  \brief  Definition of class LEDManager.
 *          For detailed description, see class header.
 *
 *  Version:    0.1
 *  Date:       2013-02-08
 *  Author:     N.Kamath
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
#ifndef LEDMANAGER_H
#define LEDMANAGER_H

//Project includes
#include <GPIOManager/Include/GPIO.h>
#include <GPIOManager/Include/GenericState.h>
#include <GPIOManager/Commands/Include/CmdSoftSwitchLEDControl.h> //!< enum definition for LEDControl_t is present here

namespace GPIOManager {

/****************************************************************************/
/**
 * \brief  This class shall be used to Manage the states of LED's associated
 *         with SoftSwitch. LED's to be controlled are connected to GPIO lines
 *         of the processor (i.Mx35).
 *         StateMachine approach is used to control LED's.Two states are
 *         provided by the class:
 *         Standby State - wherein, the system is booted up . At this state
 *         GREEN LED is OFF and RED LED is ON.
 *         InitComplete State- wherein, the system has completed its initiali-
 *         -zation of the software as well as slave devices. At this state
 *         GREEN LED is ON and RED LED is OFF.
 */
/****************************************************************************/
class LedManager : public QObject
{
    Q_OBJECT
    friend class TestGPIOThreadController; //!< Unit test
    typedef GenericState<LedManager> GenericStateTemplate;//!< typedef for LEDManager generic state
public:
    LedManager(QObject *p_Parent = 0, bool ServiceMode = false);
    ~LedManager();

    void ControlLED(const LEDControl_t LEDControl);
private:
    GPIOPin m_LEDGPIOGreen;         //!< GPIO connected to the LED on EBox
    GPIOPin m_LEDGPIORed;           //!< GPIO connected to the LED on EBox
    // State machine attributes
    QStateMachine           *mp_LEDStateMachine;       //!< StateMachine managing LED workflow
    GenericStateTemplate    *mp_StandByState;          //!< StandBy State
    GenericStateTemplate    *mp_InitComplete;          //!< State when System Init is compelete

    bool    m_SeriveMode; //!< true indicates software is in service mode.
    void Init();
    void ServiceModeInit();
    void OnEntryStandByState();
    void OnEntryInitCompleteState();
    /****************************************************************************/
    /**
     * \brief  Disable copy and assignment operator
     */
    /****************************************************************************/
    Q_DISABLE_COPY(LedManager)
private slots:
    void StartStateMachine();
signals:
    /****************************************************************************/
    /**
     * \brief  Signals init completion. Connected to main state machine signal.
     */
    /****************************************************************************/
    void InitComplete();
};

}
#endif // LEDMANAGER_H
