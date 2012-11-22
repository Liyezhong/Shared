#include "Device.h"
#include "SignalTransition.h"

namespace DeviceControl
{

typedef CSignalTransition<CDevice> CDeviceTransition;


QStringList CDevice::GetStates()
{
    QSet<QAbstractState *> states = m_machine.configuration();
    QStringList list;
    foreach (QAbstractState *value, states) {
        CState *state = dynamic_cast<CState*>(value);
        if (state != NULL)
            list << state->name();
    }
    return list;
}


CDevice::CDevice(CMockFm *fm) : mp_fm(fm)
{

    // Needed for enum arguments when QStateMachine turns signals into SignalEvents.
    // This is needed where enum is defined:
    //   Q_DECLARE_METATYPE(DeviceControl::ReturnCode_t)
    // DeviceProcessing already provides a method for this.
    qRegisterMetaType<ReturnCode_t>("ReturnCode_t");
    qRegisterMetaType<LoaderRFIDChannel_t>("LoaderRFIDChannel_t");

    // Root states: All MachineError
    // All: Init Working 
    // Init: Start Configuring Configured Initializing
    // Working: [device dependent parallel states]
    // MachineError: [internal state machine runtime error]

    // All: targetless transition: negatively ack all DCP signals by default
    // All: NeedInitialize triggers transition to Configured

    // TODO: 
    // All: ReportError Signal
    // Do we need Configure step? Keep interface lean => configure in constructor
    // Working: Stop, Resume for long activity states such as steppermotor.
    // Working: Initialize when all parallel states are in idle state. Check configuration: wait+timeout or nack when not all idle. (really needed?)
    // Nice to have: loop through all FM objects and log unexpected signals


    /////////////////////////////////////////////////////////////////
    // Set up all states
    /////////////////////////////////////////////////////////////////

    mp_All = new CState("All", &m_machine);
    mp_MachineError = new CState("MachineError", &m_machine);
    m_machine.setInitialState(mp_All);

    mp_Init = new CState("Init", mp_All);
    mp_Working = new CState("Working", mp_All);
    mp_All->setInitialState(mp_Init);

    mp_Start = new CState("Start", mp_Init);
    mp_Configuring = new CState("Configuring", mp_Init);
    mp_Configured = new CState("Configured", mp_Init);
    mp_Initializing = new CState("Initializing", mp_Init);
    mp_Init->setInitialState(mp_Start);

    // Set state for internal state machine runtime error
    // The execution of the state graph will not stop.
    m_machine.setErrorState(mp_MachineError);

    /////////////////////////////////////////////////////////////////
    // Set up all transitions
    /////////////////////////////////////////////////////////////////

    // Handle 'Configure' request
 
    mp_Start->addTransition(
        this, SIGNAL(Configure()),
        mp_Configuring );

    mp_Configuring->addTransition(
        mp_Configuring, SIGNAL(finished()),
        mp_Configured );

    // Handle 'Initialize' request

    mp_Configured->addTransition(
        this, SIGNAL(Initialize()),
        mp_Initializing );

    mp_Initializing->addTransition( new CDeviceTransition(
        mp_Initializing, SIGNAL(finished()),
        *this, &CDevice::Trans_Initializing_Working,
        mp_Working) );

    // On error jump back to Configured
    mp_Initializing->addTransition( new CDeviceTransition(
        this, SIGNAL(NeedInitialize(ReturnCode_t)),
        *this, &CDevice::Trans_Initializing_Configured,
        mp_Configured) );

    // Send Initialize NACKs by default
    mp_All->addTransition( new CDeviceTransition(
        this, SIGNAL(Initialize()),
        *this, &CDevice::Initialize_Nack) );

    // Handle MachineError
    connect( mp_MachineError, SIGNAL(entered()), this, SLOT(OnMachineErrorEntered()) );
}


bool CDevice::Trans_Initializing_Working(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportInitialize(DCL_ERR_FCT_CALL_SUCCESS);
    return true;
}

bool CDevice::Trans_Initializing_Configured(QEvent *p_Event)
{
    // NeedInitialize(ReturnCode_t)
    ReturnCode_t ReturnCode = CDeviceTransition::GetEventValue(p_Event, 0);
    emit ReportInitialize(ReturnCode);
    return true;
}

bool CDevice::Initialize_Nack(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportInitialize(DCL_ERR_INVALID_STATE);
    return true;
}

void CDevice::OnMachineErrorEntered()
{
    // When entered connect a slot which calls ReportError() 
    //   with data from error(), errorString(),
    // This is a serious implementation bug such as: NoInitialState, 
    //   NoDefaultStateInHistoryState, NoCommonAncestorForTransition, 
}

} //namespace


// vi: set ts=4 sw=4 et:

