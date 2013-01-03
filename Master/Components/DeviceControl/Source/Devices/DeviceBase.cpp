/****************************************************************************/
/*! \file   DeviceBase.cpp
 *
 *  \brief  Implementaion file for class CDeviceBase
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
#include <QFinalState>
#include "DeviceControl/Include/Devices/DeviceBase.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"

namespace DeviceControl
{
QMap<DevInstanceID_t, QString> CDeviceBase::m_DeviceName;

/****************************************************************************/
/*!
 *  \brief  Signal transition for CDeviceBase
 *
 */
/****************************************************************************/
typedef CSignalTransition<CDeviceBase> CDeviceTransition;

/****************************************************************************/
/*!
 *  \brief  To get all active states of the state machine
 *
 *  \return String list of all active states' name
 */
/****************************************************************************/
QStringList CDeviceBase::GetStates()
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

/****************************************************************************/
/*!
 *  \brief      Constructor of class CDeviceBase
 *
 *  \iparam     DeviceProc Reference of DeviceProcessing to connect required
 *              resuqest & response signals
 *
 *              FctModList Function module list of this device, to invoke
 *              function module interface functions.
 *
 *              InstanceID Device Instance ID
 */
/****************************************************************************/
CDeviceBase::CDeviceBase(const DeviceProcessing &DeviceProc, const DeviceModuleList_t &ModuleList,
                         DevInstanceID_t InstanceID) :
    m_machine(this), m_DeviceProcessing(DeviceProc), m_InstanceID(InstanceID), m_ModuleList(ModuleList)
{
    m_DeviceName[DEVICE_INSTANCE_ID_GRAPPLER_1]     = "LeftXyz";
    m_DeviceName[DEVICE_INSTANCE_ID_GRAPPLER_2]     = "RightXyz";
    m_DeviceName[DEVICE_INSTANCE_ID_LOADER]         = "Loader";
    m_DeviceName[DEVICE_INSTANCE_ID_UNLOADER]       = "Unloader";
    m_DeviceName[DEVICE_INSTANCE_ID_OVEN]           = "Oven";
    m_DeviceName[DEVICE_INSTANCE_ID_HVESSELS]       = "HeatedCuvettes";
    m_DeviceName[DEVICE_INSTANCE_ID_AGITATION]      = "Agitation";
    m_DeviceName[DEVICE_INSTANCE_ID_RACKTRANSFER]   = "RackTransfer";
    m_DeviceName[DEVICE_INSTANCE_ID_EXHAUST]        = "Exhaust";
    m_DeviceName[DEVICE_INSTANCE_ID_WATER]          = "Water";
    m_DeviceName[DEVICE_INSTANCE_ID_HOOD]           = "Hood";
    m_DeviceName[DEVICE_INSTANCE_ID_SLIDE_ID]       = "SlideId";
    m_DeviceName[DEVICE_INSTANCE_ID_COVERLINE_1]    = "Unused";
    m_DeviceName[DEVICE_INSTANCE_ID_COVERLINE_2]    = "Unused";
    m_DeviceName[DEVICE_INSTANCE_ID_RACK_HANDLING]  = "Unused";

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
    //  The Base device shall handle asynchronous ReportError
    //  signals from FMs, e.g. CAN communication or heartbeat errors.
    //  * All: eportError signals will be forwarded to the DCP,
    //    where they are collected and sent to the EventHandler.
    //  * Working: On selected errors a NeedInitialize signal will enforce 
    //    the Configured state.

    // TODO: 
    // Working: Disable => SetStatus(0), NeedInitialize
    // GetDeviceState: Shall always return state, always succeed
    // debugging: loop through all FM objects and log unexpected signals

    /////////////////////////////////////////////////////////////////
    // Set up all states
    /////////////////////////////////////////////////////////////////

    mp_All = new CState(m_DeviceName[m_InstanceID], &m_machine);
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
    //TODO: Configure Nack, DefaultNack
 
//    // Send Configure ACK
//    mp_Configured->addTransition( new CDeviceTransition(
//        mp_Configuring, SIGNAL(entered()),
//        *this, &CDeviceBase::Configure_Ack) );

    // Handle 'Initialize' request

    mp_Configured->addTransition(
        this, SIGNAL(Initialize()),
        mp_Initializing );

    mp_Initializing->addTransition( new CDeviceTransition(
        mp_Initializing, SIGNAL(finished()),
        *this, &CDeviceBase::Trans_Initializing_Working,
        mp_Working) );

    // On error jump back to Configured
    mp_Initializing->addTransition( new CDeviceTransition(
        this, SIGNAL(NeedInitialize(ReturnCode_t)),
        *this, &CDeviceBase::Trans_Initializing_Configured,
        mp_Configured) );

    // On serious error set from Working to Configured
    mp_Working->addTransition( 
        this, SIGNAL(NeedInitialize(ReturnCode_t)), mp_Configured);

    // Send Initialize NACKs by default
    mp_All->addTransition( new CDeviceTransition(
        this, SIGNAL(Initialize()),
        *this, &CDeviceBase::Initialize_Nack) );

    // Handle MachineError
    connect( mp_MachineError, SIGNAL(entered()), this, SLOT(OnMachineErrorEntered()) );

    // Connect shutdown message
    connect(&m_DeviceProcessing, SIGNAL(DeviceShutdown()), this, SLOT(Shutdown()));

    QMapIterator<QString, quint32> Iterator(m_ModuleList);
    while (Iterator.hasNext()) {
        Iterator.next();
        CModule *p_Module = m_DeviceProcessing.GetNodeFromID(GetModuleInstanceFromKey(Iterator.key()));

        if (p_Module == NULL) {
            p_Module = m_DeviceProcessing.GetFunctionModule(GetModuleInstanceFromKey(Iterator.key()));
        }
        if (p_Module != NULL) {
            m_ModuleMap[Iterator.key()] = p_Module;
        }
    }

    connect(&m_Thread, SIGNAL(started()), this, SLOT(ThreadStarted()));
    moveToThread(&m_Thread);
    m_Thread.start();
}

/****************************************************************************/
/*!
 *  \brief      To report completion of configuration.
 *
 *              Invoked when state Configuring is finished
 *
 *  \iparam     p_Event     Not used
 */
/****************************************************************************/
bool CDeviceBase::Configure_Ack(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportConfigure(DCL_ERR_FCT_CALL_SUCCESS);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To report completion of initialization.
 *
 *              Invoked when state changes from Initializing to Working
 *
 *  \iparam     p_Event     Not used
 */
/****************************************************************************/
bool CDeviceBase::Trans_Initializing_Working(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportInitialize(DCL_ERR_FCT_CALL_SUCCESS);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To report reinitialization has been triggered.
 *
 *              Invoked when state changes from Initializing to Configured
 *
 *  \iparam     p_Event     Contains ReturnCode passed by the signal
 */
/****************************************************************************/
bool CDeviceBase::Trans_Initializing_Configured(QEvent *p_Event)
{
    // NeedInitialize(ReturnCode_t)
    ReturnCode_t ReturnCode = CDeviceTransition::GetEventValue(p_Event, 0);
    emit ReportInitialize(ReturnCode);
    return true;
}

/****************************************************************************/
/*!
 *  \brief      To send NACK if Initialize signal is emitted when device is
 *              in inappropriate state
 *
 *  \iparam     p_Event     Not used
 */
/****************************************************************************/
bool CDeviceBase::Initialize_Nack(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportInitialize(DCL_ERR_INVALID_STATE);
    return true;
}

/****************************************************************************/
/*!
 *  \brief  State Machine Error.
 *
 *          Occurs due to implementation errors such as NoInitialState,
 *          NoDefaultStateInHistoryState, NoCommonAncestorForTransition.
 */
/****************************************************************************/
void CDeviceBase::OnMachineErrorEntered()
{
    // When entered connect a slot which calls ReportError() 
    //   with data from error(), errorString(),
    // This is a serious implementation bug such as: NoInitialState, 
    //   NoDefaultStateInHistoryState, NoCommonAncestorForTransition, 
}

/****************************************************************************/
/*!
 *  \brief   Returns the instance identifier of a module
 *
 *  \param   Key = Module key, as defined at the hardware specification file
 *
 *  \return  The instance ID of the module, or 0 if Key doesn't match
 */
/****************************************************************************/
quint32 CDeviceBase::GetModuleInstanceFromKey(const QString &Key)
{
    quint32 InstanceID = 0;

    if(m_ModuleList.contains(Key))
    {
        InstanceID = m_ModuleList.value(Key);
    }

    return InstanceID;
}

/****************************************************************************/
/*!
 *  \brief      Invoked when its thread is started. All child objects of the
 *              devices shall be instantiated after this event.
 *
 *  \iparam     none
 *
 *  \return     none
 */
/****************************************************************************/
void CDeviceBase::ThreadStarted()
{
    // Configure step might be needed for ProtoTest in order to
    // allow reconfiguration (FM housekeeping and connects).
    // => automatically trigger configuration from constructor
    //    in order to keep interface lean at first.

    /////////////////////////////////////////////////////////////////
    // Configuring composite states
    /////////////////////////////////////////////////////////////////

    CState *ConfigStart = new CState("ConfigStart", mp_Configuring);
    CState *ConfigDone= new CState("ConfigDone", mp_Configuring);
    QFinalState *ConfigEnd = new QFinalState(mp_Configuring);
    mp_Configuring->setInitialState(ConfigStart);
    // TODO: add configuration work
    ConfigStart->addTransition(new CDeviceTransition(ConfigStart, SIGNAL(entered()),
                                                     *this, &CDeviceBase::Trans_Configure, ConfigDone));
    // Send Configure ACK
    ConfigDone->addTransition( new CDeviceTransition(
        ConfigDone, SIGNAL(entered()),
        *this, &CDeviceBase::Configure_Ack,
        ConfigEnd));

    m_machine.start();
}

/****************************************************************************/
/*!
 *  \brief   To exit thread when device shutdown event is triggered
 *
 */
/****************************************************************************/
void CDeviceBase::Shutdown()
{
    if (m_Thread.isRunning())
    {
        m_Thread.exit(0);
    }
}

CDeviceBase::~CDeviceBase()
{
    if (m_Thread.isRunning())
    {
        m_Thread.exit(0);

        //! \todo should we wait for thread to exit?
        m_Thread.wait(100);
    }
}

} //namespace


// vi: set ts=4 sw=4 et:

