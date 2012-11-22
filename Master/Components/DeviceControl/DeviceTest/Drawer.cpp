#include "Drawer.h"
#include <QFinalState>
#include "SignalTransition.h"

#include "FmStepperMotorInitialize.h"
#include "DrawerLed.h"
#include "ReadRackRfid.h"
#include "DrawerMotion.h"

namespace DeviceControl
{

typedef CSignalTransition<CDrawer> CDrawerTransition;

CDrawer::CDrawer(CMockFm *fm) : CDevice(fm)
{
    // TODO: Do we need configure step? 
    //   => When integrating, move all configuration, 
    //      FM housekeeping and connects to constructor! 

    /////////////////////////////////////////////////////////////////
    // Configuring composite states
    /////////////////////////////////////////////////////////////////

    CState *ConfigStart = new CState("ConfigStart", mp_Configuring);
    QFinalState *ConfigEnd = new QFinalState(mp_Configuring);
    mp_Configuring->setInitialState(ConfigStart);
    // TODO: add configuration work
    ConfigStart->addTransition(ConfigEnd);

    /////////////////////////////////////////////////////////////////
    // Initializing 
    /////////////////////////////////////////////////////////////////

    CFmStepperMotorInitialize *p_InitStepper = 
        new CFmStepperMotorInitialize(fm, "InitStepper", mp_Initializing);
    CDrawerLed *p_InitLed = 
        new CDrawerLed(fm, true, fm, false, "InitLed", mp_Initializing);
    QFinalState *p_InitEnd = new QFinalState(mp_Initializing);
    mp_Initializing->setInitialState(p_InitStepper);

    p_InitStepper->addTransition( p_InitStepper, SIGNAL(finished()), p_InitLed );
    p_InitLed->addTransition( p_InitLed, SIGNAL(finished()), p_InitEnd );

    connect(p_InitStepper, SIGNAL(NeedInitialize(ReturnCode_t)),
            this,          SIGNAL(NeedInitialize(ReturnCode_t)));

    connect(p_InitLed,     SIGNAL(LedError(ReturnCode_t)),
            this,          SIGNAL(NeedInitialize(ReturnCode_t)));

    /////////////////////////////////////////////////////////////////
    // Working composite states
    /////////////////////////////////////////////////////////////////

    mp_Working->setChildMode(QState::ParallelStates);

    // ReadRackRfid
    CReadRackRfid *p_ReadRackRfid = new CReadRackRfid(fm, "ReadRackRfid", mp_Working);

    connect(this,           SIGNAL(ReadRackRfid(LoaderRFIDChannel_t)),
            p_ReadRackRfid, SIGNAL(ReadRackRfid(LoaderRFIDChannel_t)));
    connect(p_ReadRackRfid, SIGNAL(ReportReadRackRfid(ReturnCode_t, LoaderRFIDChannel_t, quint32, quint32)),
            this,           SIGNAL(ReportReadRackRfid(ReturnCode_t, LoaderRFIDChannel_t, quint32, quint32)));

    // DrawerMotion
    CDrawerMotion *p_DrawerMotion = new CDrawerMotion(fm, fm, fm, fm, "DrawerMotion", mp_Working);

    connect(this,           SIGNAL(Block()),
            p_DrawerMotion, SIGNAL(Block()));
    connect(this,           SIGNAL(Unblock()),
            p_DrawerMotion, SIGNAL(Unblock()));
    connect(p_DrawerMotion, SIGNAL(ReportBlock(ReturnCode_t)),
            this,           SIGNAL(ReportBlock(ReturnCode_t)));
    connect(p_DrawerMotion, SIGNAL(ReportUnblock(ReturnCode_t)),
            this,           SIGNAL(ReportUnblock(ReturnCode_t)));

    // Send Nack for all requests by default:
    // This is done by adding targetless transitions to upper layer states,
    // which can be overridden by the child states.

    // Caution: 
    // An event is distributed to all children of a parallel composite state.
    // If the event is not consumed by all children, the event is not overridden.
    // Workarounds
    // Exclude mp_Working state from source states for DefaultNack,
    // i.e. use mp_Init and only actual child state of mp_Working 
    // Send Nacks from Device level for all commands.

    mp_Init->addTransition( new CDrawerTransition(
        p_ReadRackRfid, SIGNAL(ReadRackRfid(LoaderRFIDChannel_t)),
        *this, &CDrawer::DefaultNackReadRackRfid) );
    p_ReadRackRfid->addTransition( new CDrawerTransition(
        p_ReadRackRfid, SIGNAL(ReadRackRfid(LoaderRFIDChannel_t)),
        *this, &CDrawer::DefaultNackReadRackRfid) );

    mp_Init->addTransition( new CDrawerTransition(
        p_DrawerMotion, SIGNAL(Block()),
        *this, &CDrawer::DefaultNackBlock) );
    p_DrawerMotion->addTransition( new CDrawerTransition(
        p_DrawerMotion, SIGNAL(Block()),
        *this, &CDrawer::DefaultNackBlock) );

    mp_Init->addTransition( new CDrawerTransition(
        p_DrawerMotion, SIGNAL(Unblock()),
        *this, &CDrawer::DefaultNackUnblock) );
    p_DrawerMotion->addTransition( new CDrawerTransition(
        p_DrawerMotion, SIGNAL(Unblock()),
        *this, &CDrawer::DefaultNackUnblock) );

    m_machine.start();
}


bool CDrawer::DefaultNackReadRackRfid(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportReadRackRfid(DCL_ERR_INVALID_STATE, LOADER_ID_RFID_UNDEF, 0, 0);
    return true;
}
bool CDrawer::DefaultNackBlock(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportBlock(DCL_ERR_INVALID_STATE);
    return true;
}

bool CDrawer::DefaultNackUnblock(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportUnblock(DCL_ERR_INVALID_STATE);
    return true;
}


} //namespace


// vi: set ts=4 sw=4 et:

