/****************************************************************************/
/*! \file   MoveOpenClosed.cpp
 *
 *  \brief  Implementation file for class CMoveOpenClosed
 *          To perform move, open and closed operations for various devices
 *
 *  \version  0.1
 *  \date     2012-09-26
 *  \author   Torsten Scheck
 *
 *  \b Description:
 *
 *   N/A
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/
#include <QDebug>
#include <QStateMachine>

#include "DeviceControl/Include/Devices/DeviceBase.h"
#include "DeviceControl/Include/Devices/SetDrawerLed.h"
#include "DeviceControl/Include/Devices/MoveOpenClosed.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Signal transition for CMoveOpenClosed
 */
/****************************************************************************/
typedef CSignalTransition<CMoveOpenClosed> CDrawerMotionTransition;

/****************************************************************************/
/*!
 *  \brief      Constructor of class CMoveOpenClosed
 *              To be used by Loader & Unloader devices
 *
 *  \iparam     p_FmMotor       Stepper motor
 *              p_FmDiButton    Push button
 *              p_FmDoLedGreen  Green Led
 *              mp_FmDoLedRed   Red Led
 *
 *              Name            Name of the state
 *
 *              p_Parent        Parent of the state
 */
/****************************************************************************/
CMoveOpenClosed::CMoveOpenClosed(
    CStepperMotor *p_FmMotor,
    CDigitalInput *p_FmDiButton,
    CDigitalOutput *p_FmDoLedGreen,
    CDigitalOutput *p_FmDoLedRed,
    const QString &Name, 
    QState *p_Parent) :
        CState(Name, p_Parent), 
        mp_FmMotor(p_FmMotor),
        mp_FmDiButton(p_FmDiButton),
        mp_FmDoLedGreen(p_FmDoLedGreen),
        mp_FmDoLedRed(p_FmDoLedRed)
{
    //! \todo: remove p_FmMotor, not used anywhere
    
    CommandActive = false;
    m_PositionOpen        = DEFAULT_OPEN_POS;
    m_PositionClosed      = DEFAULT_CLOSE_POS;
    m_MotionProfileOpen   = DEFAULT_OPEN_PROFILE;
    m_MotionProfileClosed = DEFAULT_CLOSE_PROFILE;

    // Idle and Active parent states don't fit here, as their children 
    // would jump in and out and the initial state was ambiguous.
    // We can check the state machine's configuration in the transition callback, 
    // if we want to limit transitions to active states. 
    // Or we could use foreach loops for the creation.

    // stepper states
    CState *p_Closed = new CState("Closed", this);
    CState *p_Opening = new CState("Opening", this);
    CState *p_Open = new CState("Open", this);
    CState *p_Closing = new CState("Closing", this);
    CState *p_Intermediate = new CState("Intermediate", this);

    // blocking states
    CSetDrawerLed *p_Blocking =
        new CSetDrawerLed(mp_FmDoLedGreen, false, mp_FmDoLedRed, true, "Blocking", this);
    CState *p_Blocked = new CState("Blocked", this);
    CSetDrawerLed *p_Unblocking =
        new CSetDrawerLed(mp_FmDoLedGreen, true, mp_FmDoLedRed, false, "Unblocking", this);

    // Set Closed as initial state
    setInitialState(p_Closed);

    // stepper transitions button
    p_Closed->addTransition( new CDrawerMotionTransition(
        mp_FmDiButton, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)), 
        *this, &CMoveOpenClosed::Trans_Closed_Opening_Button,
        p_Opening ));
    p_Open->addTransition( new CDrawerMotionTransition(
        mp_FmDiButton, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)), 
        *this, &CMoveOpenClosed::Trans_Open_Closing_Button,
        p_Closing ));
    p_Intermediate->addTransition( new CDrawerMotionTransition(
        mp_FmDiButton, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)), 
        *this, &CMoveOpenClosed::Trans_Open_Closing_Button,
        p_Closing ));

    // stepper transitions command request
    p_Closed->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(Open()),
        *this, &CMoveOpenClosed::Trans_Closed_Opening_Command,
        p_Opening ));
    p_Closed->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(Close()),
        *this, &CMoveOpenClosed::Trans_Open_Closing_Command));

    p_Open->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(Close()),
        *this, &CMoveOpenClosed::Trans_Open_Closing_Command,
        p_Closing ));
    p_Open->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(Open()),
        *this, &CMoveOpenClosed::DefaultNackOpen));

    p_Opening->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(MoveAck()),
        *this, &CMoveOpenClosed::Trans_Opening_Open,
        p_Open ));
    p_Closing->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(MoveAck()),
        *this, &CMoveOpenClosed::Trans_Closing_Closed,
        p_Closed ));

    // TODO Nack on abort?
    p_Opening->addTransition(
        this, SIGNAL(ReportAbort(ReturnCode_t)), p_Intermediate );
    p_Closing->addTransition(
        this, SIGNAL(ReportAbort(ReturnCode_t)), p_Intermediate );

    p_Intermediate->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(Open()),
        *this, &CMoveOpenClosed::Trans_Closed_Opening_Command,
        p_Opening ));
    p_Intermediate->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(Close()),
        *this, &CMoveOpenClosed::Trans_Open_Closing_Command,
        p_Closing ));

    // stepper error transitions
    p_Opening->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(StepperError(ReturnCode_t)),
        *this, &CMoveOpenClosed::NackOpen,
        p_Intermediate ));
    p_Closing->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(StepperError(ReturnCode_t)),
        *this, &CMoveOpenClosed::NackClose,
        p_Intermediate ));


    // blocking transitions
    p_Closed->addTransition( this, SIGNAL(Block()), p_Blocking );
    p_Blocking->addTransition( new CDrawerMotionTransition(
        p_Blocking, SIGNAL(finished()), 
        *this, &CMoveOpenClosed::AckBlock,
        p_Blocked ));
    p_Blocked->addTransition( this, SIGNAL(Unblock()), p_Unblocking );
    p_Unblocking->addTransition( new CDrawerMotionTransition(
        p_Unblocking, SIGNAL(finished()), 
        *this, &CMoveOpenClosed::AckUnblock,
        p_Closed ));

    // blocking error transitions
    p_Blocking->addTransition( new CDrawerMotionTransition(
        p_Blocking, SIGNAL(LedError(ReturnCode_t)),
        *this, &CMoveOpenClosed::NackBlock,
        p_Closed) );
    p_Unblocking->addTransition( new CDrawerMotionTransition(
        p_Blocking, SIGNAL(LedError(ReturnCode_t)),
        *this, &CMoveOpenClosed::NackUnblock,
        p_Blocked) );
}

/****************************************************************************/
/*!
 *  \brief      Constructor of class CMoveOpenClosed
 *              To be used by devices other than Loader and Unloader such as
 *              Agitation, Oven, Rack Transfer, etc.
 *
 *  \iparam     p_FmMotor       Stepper motor
 *
 *              Name            Name of the state
 *
 *              p_Parent        Parent of the state
 */
/****************************************************************************/
CMoveOpenClosed::CMoveOpenClosed(
    CStepperMotor *p_FmMotor,
    const QString &Name,
    QState *p_Parent) :
        CState(Name, p_Parent),
        mp_FmMotor(p_FmMotor),
        mp_FmDiButton(NULL),
        mp_FmDoLedGreen(NULL),
        mp_FmDoLedRed(NULL)
{
    CommandActive = false;
    m_PositionOpen        = DEFAULT_OPEN_POS;
    m_PositionClosed      = DEFAULT_CLOSE_POS;
    m_MotionProfileOpen   = DEFAULT_OPEN_PROFILE;
    m_MotionProfileClosed = DEFAULT_CLOSE_PROFILE;

    // Idle and Active parent states don't fit here, as their children
    // would jump in and out and the initial state was ambiguous.
    // We can check the state machine's configuration in the transition callback,
    // if we want to limit transitions to active states.
    // Or we could use foreach loops for the creation.

    // stepper states
    CState *p_Closed = new CState("Closed", this);
    CState *p_Opening = new CState("Opening", this);
    CState *p_Open = new CState("Open", this);
    CState *p_Closing = new CState("Closing", this);
    CState *p_Intermediate = new CState("Intermediate", this);

    setInitialState(p_Closed);

    // stepper transitions command request
    p_Closed->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(Open()),
        *this, &CMoveOpenClosed::Trans_Closed_Opening_Command,
        p_Opening ));
    p_Closed->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(Close()),
        *this, &CMoveOpenClosed::DefaultNackClose));

    p_Open->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(Close()),
        *this, &CMoveOpenClosed::Trans_Open_Closing_Command,
        p_Closing ));
    p_Open->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(Open()),
        *this, &CMoveOpenClosed::DefaultNackOpen));

    p_Opening->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(MoveAck()),
        *this, &CMoveOpenClosed::Trans_Opening_Open,
        p_Open ));
    p_Closing->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(MoveAck()),
        *this, &CMoveOpenClosed::Trans_Closing_Closed,
        p_Closed ));

    // TODO Nack on abort?
    p_Opening->addTransition(
        this, SIGNAL(ReportAbort(ReturnCode_t)), p_Intermediate );
    p_Closing->addTransition(
        this, SIGNAL(ReportAbort(ReturnCode_t)), p_Intermediate );

    p_Intermediate->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(Open()),
        *this, &CMoveOpenClosed::Trans_Closed_Opening_Command,
        p_Opening ));
    p_Intermediate->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(Close()),
        *this, &CMoveOpenClosed::Trans_Open_Closing_Command,
        p_Closing ));

    // stepper error transitions
    p_Opening->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(StepperError(ReturnCode_t)),
        *this, &CMoveOpenClosed::NackOpen,
        p_Intermediate ));
    p_Closing->addTransition( new CDrawerMotionTransition(
        this, SIGNAL(StepperError(ReturnCode_t)),
        *this, &CMoveOpenClosed::NackClose,
        p_Intermediate ));
}

/****************************************************************************/
/*!
 *  \brief      To trigger open on button press
 *
 *  \iparam     p_Event refers to arguements passed by the signal
 *
 *  \return     return value of Trans_Closed_Opening().
 */
/****************************************************************************/
bool CMoveOpenClosed::Trans_Closed_Opening_Button(QEvent *p_Event)
{
    CommandActive = false;
    if (! ButtonPressed(p_Event)) {
        return false;
    }
    return Trans_Closed_Opening(p_Event);
}

/****************************************************************************/
/*!
 *  \brief      To trigger open on reception of open command
 *
 *  \iparam     p_Event refers to arguements passed by the signal
 *
 *  \return     return value of Trans_Closed_Opening().
 */
/****************************************************************************/
bool CMoveOpenClosed::Trans_Closed_Opening_Command(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    CommandActive = true;
    return Trans_Closed_Opening(p_Event);
}

/****************************************************************************/
/*!
 *  \brief      To move to open position
 *
 *  \iparam     p_Event refers to arguements passed by the signal
 *
 *  \return     true
 */
/****************************************************************************/
bool CMoveOpenClosed::Trans_Closed_Opening(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit Move(m_PositionOpen, m_MotionProfileOpen);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To triggerclose on button press
 *
 *  \iparam     p_Event refers to arguements passed by the signal
 *
 *  \return     return value of Trans_Open_Closing().
 */
/****************************************************************************/
bool CMoveOpenClosed::Trans_Open_Closing_Button(QEvent *p_Event)
{
    CommandActive = false;
    if (! ButtonPressed(p_Event)) {
        return false;
    }
    return Trans_Open_Closing(p_Event);
}

/****************************************************************************/
/*!
 *  \brief      To trigger close on reception of close command
 *
 *  \iparam     p_Event refers to arguements passed by the signal
 *
 *  \return     return value of Trans_Open_Closing().
 */
/****************************************************************************/
bool CMoveOpenClosed::Trans_Open_Closing_Command(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    CommandActive = true;
    return Trans_Open_Closing(p_Event);
}

/****************************************************************************/
/*!
 *  \brief      To move to close position
 *
 *  \iparam     p_Event refers to arguements passed by the signal
 *
 *  \return     true
 */
/****************************************************************************/
bool CMoveOpenClosed::Trans_Open_Closing(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit Move(m_PositionClosed, m_MotionProfileClosed);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To validate button press event
 *
 *  \iparam     p_Event refers to arguements passed by the signal
 *
 *  \return     true, if return code is successful and button is released, else false
 */
/****************************************************************************/
bool CMoveOpenClosed::ButtonPressed(QEvent *p_Event)
{
    // ReportActInputValue(quint32 InstanceID, ReturnCode_t ReturnCode,, quint16 InputValue)
    ReturnCode_t ReturnCode = CDrawerMotionTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        // async: emit ReportError( ... DCL_ERR_);
        return false;
    }
    quint16 value;
    if (!CDrawerMotionTransition::GetEventValue(p_Event, 2, value)) {
        // async: emit ReportError( ... DCL_ERR_INVALID_PARAM);
        return false;
    }
    return (BUTTON_RELEASED == value);
}

/****************************************************************************/
/*!
 *  \brief      To acknowledge open command & trigger opened() event
 *
 *  \iparam     p_Event unused
 *
 *  \return     true
 */
/****************************************************************************/
bool CMoveOpenClosed::Trans_Opening_Open(QEvent *p_Event)
{
    // MoveAck()
    Q_UNUSED(p_Event)
    if (CommandActive) {
        emit ReportOpen(DCL_ERR_FCT_CALL_SUCCESS);
    }
    emit Opened();
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To acknowledge close command& trigger closed() event
 *
 *  \iparam     p_Event unused
 *
 *  \return     true
 */
/****************************************************************************/
bool CMoveOpenClosed::Trans_Closing_Closed(QEvent *p_Event)
{
    // MoveAck()
    Q_UNUSED(p_Event)
    if (CommandActive) {
        emit ReportClose(DCL_ERR_FCT_CALL_SUCCESS);
    }
    emit Closed();
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To acknowledge negative on error while opening
 *
 *  \iparam     p_Event refers to arguements passed by the signal
 *
 *  \return     true
 */
/****************************************************************************/
bool CMoveOpenClosed::NackOpen(QEvent *p_Event)
{
    // StepperError(ReturnCode_t ReturnCode)
    if (CommandActive) {
        ReturnCode_t ReturnCode = CDrawerMotionTransition::GetEventValue(p_Event, 0);
        emit ReportOpen(ReturnCode);
    }
    // Device will also emit ReportError due to StepperError
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To acknowledge negative on error while closing
 *
 *  \iparam     p_Event refers to arguements passed by the signal
 *
 *  \return     true
 */
/****************************************************************************/
bool CMoveOpenClosed::NackClose(QEvent *p_Event)
{
    // StepperError(ReturnCode_t ReturnCode)
    if (CommandActive) {
        ReturnCode_t ReturnCode = CDrawerMotionTransition::GetEventValue(p_Event, 0);
        emit ReportClose(ReturnCode);
    }
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To acknowledge block command
 *
 *  \iparam     p_Event unused
 *
 *  \return     true
 */
/****************************************************************************/
bool CMoveOpenClosed::AckBlock(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportBlock(DCL_ERR_FCT_CALL_SUCCESS);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To acknowledge unblock command
 *
 *  \iparam     p_Event unused
 *
 *  \return     true
 */
/****************************************************************************/
bool CMoveOpenClosed::AckUnblock(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportUnblock(DCL_ERR_FCT_CALL_SUCCESS);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To acknowledge negative for block command
 *
 *  \iparam     p_Event unused
 *
 *  \return     true
 */
/****************************************************************************/
bool CMoveOpenClosed::NackBlock(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CDrawerMotionTransition::GetEventValue(p_Event, 0);
    emit ReportBlock(ReturnCode);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To acknowledge negative for unblock command
 *
 *  \iparam     p_Event unused
 *
 *  \return     true
 */
/****************************************************************************/
bool CMoveOpenClosed::NackUnblock(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CDrawerMotionTransition::GetEventValue(p_Event, 0);
    emit ReportUnblock(ReturnCode);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To acknowledge negative if open is requested in wrong state
 *
 *  \iparam     p_Event unused
 *
 *  \return     true
 */
/****************************************************************************/
bool CMoveOpenClosed::DefaultNackOpen(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportOpen(DCL_ERR_INVALID_STATE);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To acknowledge negative if close is requested in wrong state
 *
 *  \iparam     p_Event unused
 *
 *  \return     true
 */
/****************************************************************************/
bool CMoveOpenClosed::DefaultNackClose(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportClose(DCL_ERR_INVALID_STATE);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      Setter function to set open position
 *
 *  \iparam     Position Open Position
 */
/****************************************************************************/
void CMoveOpenClosed::SetOpenPosition(quint32 Position)
{
    m_PositionOpen = Position;
}

/****************************************************************************/
/*!
 *  \brief      Setter function to set close position
 *
 *  \iparam     Position Close Position
 */
/****************************************************************************/
void CMoveOpenClosed::SetClosePosition(quint32 Position)
{
    m_PositionClosed = Position;
}

/****************************************************************************/
/*!
 *  \brief      Setter function to set open profile
 *
 *  \iparam     Profile Open Profile
 */
/****************************************************************************/
void CMoveOpenClosed::SetOpenProfile(quint32 Profile)
{
    m_MotionProfileOpen = Profile;
}

/****************************************************************************/
/*!
 *  \brief      Setter function to set close profile
 *
 *  \iparam     Profile Close Profile
 */
/****************************************************************************/
void CMoveOpenClosed::SetCloseProfile(quint32 Profile)
{
    m_MotionProfileClosed = Profile;
}

} //namespace

// vi: set ts=4 sw=4 et:
