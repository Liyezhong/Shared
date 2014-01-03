/****************************************************************************/
/*! \file LEDManager.cpp
 *
 *  \brief LEDManager Implementation
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.02.2013
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
#include <GPIOManager/Include/LEDManager.h>
#include <EventHandler/Include/StateHandler.h>
#include <GPIOManager/Include/GPIOManagerEventCodes.h>
#include <Global/Include/Utils.h>

//lint -sem(GPIOManager::LedManager::ServiceModeInit,initializer)
namespace  GPIOManager {
const quint32 GREEN_LED_GPIO_NUMBER     = 47;    //!< GPIO2_15 of the processor (i.Mx35 EBox)
const quint32 RED_LED_GPIO_NUMBER       = 49;    //!< GPIO2_17 of the processor (i.Mx35 EBox)

/****************************************************************************/
/*!
 *  \brief    Constructor
 *  \iparam p_Parent  - Parent object
 *  \iparam ServiceMode - true indicates S/W is in service mode.
 *
 */
/****************************************************************************/
LedManager::LedManager(QObject *p_Parent, bool ServiceMode)
    : m_LEDGPIOGreen(GREEN_LED_GPIO_NUMBER, DIRECTION_IS_OUTPUT)
    , m_LEDGPIORed(RED_LED_GPIO_NUMBER, DIRECTION_IS_OUTPUT)
    , mp_LEDStateMachine(NULL)
    , mp_StandByState(NULL)
    , mp_InitComplete(NULL)
    , m_SeriveMode(ServiceMode)
{
    //! This is required so that our object is on the right thread
    setParent(p_Parent);
    if (!m_SeriveMode) {
        Init();
    }
    else {
        ServiceModeInit();
    }
}

/****************************************************************************/
/*!
 *  \brief  Destructor
 */
/****************************************************************************/
LedManager::~LedManager()
{
    /*! State machine objects are not deleted
     *  Why?
     *  we have set Parent as "this" object and when parent goes out
     *  of scope qt's dynamic object management will take care of deleting
     *  child objects :)
     */
}

/****************************************************************************/
/*!
 *  \brief  Function to control Green & Red LED.
 *  \iparam LEDControl = Numeric constant to control LED's
 */
/****************************************************************************/
void LedManager::ControlLED(const LEDControl_t LEDControl)
{
    switch (LEDControl) {
        case GREEN_LED_ON:
            m_LEDGPIOGreen.SetValue(0);
            break;
        case GREEN_LED_OFF:
            m_LEDGPIOGreen.SetValue(1);
            break;
        case RED_LED_ON:
            m_LEDGPIORed.SetValue(0);
            break;
        case RED_LED_OFF:
            m_LEDGPIORed.SetValue(1);
            break;
        case GREEN_RED_LED_ON:
            m_LEDGPIOGreen.SetValue(0);
            m_LEDGPIORed.SetValue(0);
            break;
        case GREEN_RED_LED_OFF:
            m_LEDGPIOGreen.SetValue(1);
            m_LEDGPIORed.SetValue(1);
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief  Initialize the Object. Creates, Initializes & Starts statemachine
 */
/****************************************************************************/
void LedManager::Init()
{

    //! Create StateMachine
    mp_LEDStateMachine = new QStateMachine();
    /*! Parent is set so that the statemachine and its states are in the right thread object.
     *  \warning : Set Parent to objects created in heap in this function (or before this
     *  Object is moved to the Thread)
     */
    mp_LEDStateMachine->setParent(this);

    //! Create States
    mp_StandByState = new GenericStateTemplate(*this, &LedManager::OnEntryStandByState);
    mp_StandByState->setParent(this);
    mp_StandByState->setObjectName("StandByState");

    mp_InitComplete = new GenericStateTemplate(*this, &LedManager::OnEntryInitCompleteState);
    mp_InitComplete->setParent(this);
    mp_InitComplete->setObjectName("InitCompleteState");

    //! Build the state machine
    mp_LEDStateMachine->addState(mp_StandByState);
    mp_LEDStateMachine->addState(mp_InitComplete);
    mp_LEDStateMachine->setInitialState(mp_StandByState);
    //! Add transitions
    mp_StandByState->addTransition(this, SIGNAL(InitComplete()), mp_InitComplete);

}

/****************************************************************************/
/*!
 *  \brief  Initializes LED's for Service mode.
 */
/****************************************************************************/
void LedManager::ServiceModeInit()
{
    m_LEDGPIOGreen.SetValue(0);
    m_LEDGPIORed.SetValue(0);
}

/****************************************************************************/
/*!
 *  \brief  OnEntry implementation for StandBy state
 */
/****************************************************************************/
void LedManager::OnEntryStandByState()
{
    if (!m_SeriveMode) {
        //! Glow Red LED Only, Green is off
        m_LEDGPIOGreen.SetValue(1);
        m_LEDGPIORed.SetValue(0);
    }
    else {
        m_LEDGPIOGreen.SetValue(1);
        m_LEDGPIORed.SetValue(1);
    }
}

/****************************************************************************/
/*!
 *  \brief  OnEntry implementation for InitComplete State
 */
/****************************************************************************/
void LedManager::OnEntryInitCompleteState()
{
    //! Glow green LED Only, Red is off
    m_LEDGPIOGreen.SetValue(0);
    m_LEDGPIORed.SetValue(1);
}

/****************************************************************************/
/*!
 *  \brief  Starts LED  State Machine
 */
/****************************************************************************/
void LedManager::StartStateMachine()
{
    if (mp_LEDStateMachine) {
        mp_LEDStateMachine->start();
    }
}

}//End of namespace GPIOManager
