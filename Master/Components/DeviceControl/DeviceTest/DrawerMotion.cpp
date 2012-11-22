#include "Device.h"
#include "MockFm.h"
#include "DrawerLed.h"
#include "DrawerMotion.h"
#include "SignalTransition.h"
#include "State.h"

#include <QDebug>
#include <QStateMachine>

/*

      Blocked

Blocking    Unblocking

      Closed 
   
 pressed()   AckPos

Opening     Closing

  AckPos    pressed()

      Open

*/

namespace DeviceControl
{

typedef CSignalTransition<CDrawerMotion> CDrawerMotionTransition;

CDrawerMotion::CDrawerMotion(
    CMockFm *p_FmMotor, 
    CMockFm *p_FmDiButton, 
    CMockFm *p_FmDoLedGreen, 
    CMockFm *p_FmDoLedRed, 
    const QString &Name, 
    QState *p_Parent) :
        CState(Name, p_Parent), 
        mp_FmMotor(p_FmMotor), 
        mp_FmDiButton(p_FmDiButton), 
        mp_FmDoLedGreen(p_FmDoLedGreen),
        mp_FmDoLedRed(p_FmDoLedRed)
{
    // Idle and Active parent states don't fit here, as they are no real composite states.
    // We can check the state machine's configuration in the transition callback, 
    // if we want to limit transitions to active states. Or use foreach loops for the creation.
    CState *p_Closed = new CState("Closed", this);
    //CState *p_Opening = new CState("Opening", this);
    //CState *p_Open = new CState("Open", this);
    //CState *p_Closing = new CState("Closing", this);
    CDrawerLed *p_Blocking =
        new CDrawerLed(p_FmDoLedGreen, false, p_FmDoLedRed, true, "Blocking", this);
    CState *p_Blocked = new CState("Blocked", this);
    CDrawerLed *p_Unblocking =
        new CDrawerLed(p_FmDoLedGreen, true, p_FmDoLedRed, false, "Unblocking", this);
    setInitialState(p_Closed);

    p_Closed->addTransition( this, SIGNAL(Block()), p_Blocking );
    p_Blocking->addTransition( new CDrawerMotionTransition(
        p_Blocking, SIGNAL(finished()), 
        *this, &CDrawerMotion::AckBlock,
        p_Blocked ));
    p_Blocked->addTransition( this, SIGNAL(Unblock()), p_Unblocking );
    p_Unblocking->addTransition( new CDrawerMotionTransition(
        p_Unblocking, SIGNAL(finished()), 
        *this, &CDrawerMotion::AckUnblock,
        p_Closed ));

    p_Blocking->addTransition( new CDrawerMotionTransition(
        p_Blocking, SIGNAL(LedError(ReturnCode_t)),
        *this, &CDrawerMotion::NackBlock,
        p_Closed) );
    p_Unblocking->addTransition( new CDrawerMotionTransition(
        p_Blocking, SIGNAL(LedError(ReturnCode_t)),
        *this, &CDrawerMotion::NackUnblock,
        p_Blocked) );

}

bool CDrawerMotion::AckBlock(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportBlock(DCL_ERR_INVALID_STATE);
    return true;
}

bool CDrawerMotion::AckUnblock(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportUnblock(DCL_ERR_INVALID_STATE);
    return true;
}

bool CDrawerMotion::NackBlock(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CDrawerMotionTransition::GetEventValue(p_Event, 0);
    emit ReportBlock(ReturnCode);
    return true;
}

bool CDrawerMotion::NackUnblock(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CDrawerMotionTransition::GetEventValue(p_Event, 0);
    emit ReportUnblock(ReturnCode);
    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:
